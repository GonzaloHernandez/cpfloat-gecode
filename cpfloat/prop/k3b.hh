/*
 *  Authors:
 *     Gonzalo Hernández <gonzalohernandez@udenar.edu.co>
 *     Gustavo Gutierrez
 *
 *  Year of last major update
 *     2011
 * 
 *  This file is a complement of Gecode, the generic constraint
 *  development environment:
 *     http://www.gecode.org
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __CPFLOAT_PROP_K3B_HH__
#define __CPFLOAT_PROP_K3B_HH__

#include <cpfloat/cpfloat.hh>
#include <cpfloat/expression.hh>

namespace MPG { namespace CPFloat { namespace Prop {

/**
 * \brief Propagates: \f$ exp = exp \f$
 * \ingroup SetProp
 */
class K3B : public Gecode::Propagator {
  static const int ITER  = 1000;
  static const int SPLIT = 5;
protected:
  Constraint& cst_;
public:
  /// Constructor for the propagator \f$ K3B(cst) \f$
  K3B(Gecode::Home home, Constraint& cst)
    : Gecode::Propagator(home), cst_(cst) {
    cst_.subscribe(home,*this,CPFloat::PC_CPFLOAT_BND);
  }
  /// Propagator posting
  static Gecode::ExecStatus post(Gecode::Home home, Constraint& cst) {
    /// \todo Can we do some processing here and decide to not to post
    /// the constraint?
    (void) new (home) K3B(home,cst);
    return Gecode::ES_OK;
  }
  /// Propagator disposal
  virtual size_t dispose(Gecode::Space& home) {
    cst_.cancel(home,*this,CPFloat::PC_CPFLOAT_BND);
    delete &cst_;
    (void) Propagator::dispose(home);
    return sizeof(*this);
  }
  /// Copy constructor
  K3B(Gecode::Space& home, bool share, K3B& p)
    : Gecode::Propagator(home,share,p), cst_(p.cst_.cloneToUpdate()) {
    cst_.update(home,share,p.cst_);
  }
  /// Copy
  virtual Gecode::Propagator* copy(Gecode::Space& home, bool share) {
    return new (home) K3B(home,share,*this);
  }
  /// Cost
  virtual Gecode::PropCost cost(const Gecode::Space&,
                                const Gecode::ModEventDelta&) const {
    return Gecode::PropCost::binary(Gecode::PropCost::LO);
  }
  /// Main propagation algorithm
  virtual Gecode::ExecStatus propagate(Gecode::Space& home,
                                       const Gecode::ModEventDelta&) {
    ViewArray<CPFloatView> views(home, cst_.countViews());
    cst_.collectViews(views);
    views.unique(home);
    for (int i=0; i<views.size(); i++) {
      lnar(home,views[i],0.05);
      rnar(home,views[i],0.05);
    }
    return Gecode::ES_NOFIX;
  }

  Gecode::ExecStatus lnar(Space& home, CPFloatView& view, BoundType precision) {
    int iter = 0;
    BoundType epsilon;
    BoundType length = view.lub() - view.glb();
    do {
      epsilon = (length) / 5.0;
      CPFloatView testview(CPFloatVar(home, view.glb(), add_hi(view.glb(),epsilon)));
      Constraint& testcst = cst_.cloneWithReplace(home,view,testview);
      testcst.evaluate();
      if (Gecode::me_failed(testcst.propagate(home))) {
        if (Gecode::me_failed(view.geq(home,testview.lub()))) {
          delete &testcst;
          return Gecode::ES_FAILED;
        }
        else {
          length = view.lub() - view.glb();
        }
      }
      else {
        length -= epsilon;
      }
      iter++;
      delete &testcst;
    } while (epsilon>=precision && iter < 1000);

    return Gecode::ES_NOFIX;
  }

  Gecode::ExecStatus rnar(Space& home, CPFloatView& view, BoundType precision) {
    int iter = 0;
    BoundType epsilon;
    BoundType length = view.lub() - view.glb();
    do {
      epsilon = (length) / 5.0;
      CPFloatView testview(CPFloatVar(home, sub_hi(view.lub(),epsilon) , view.lub() ));
      Constraint& testcst = cst_.cloneWithReplace(home,view,testview);
      testcst.evaluate();
      if (Gecode::me_failed(testcst.propagate(home))) {
        if (Gecode::me_failed(view.leq(home,testview.glb()))) {
          delete &testcst;
          return Gecode::ES_FAILED;
        }
        else {
          length = view.lub() - view.glb();
        }
      }
      else {
        length -= epsilon;
      }
      iter++;
      delete &testcst;
    } while (epsilon>=precision && iter < 1000);

    return Gecode::ES_NOFIX;
  }

};
}}}
#endif
