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

#ifndef __CPFLOAT_PROP_SUBTRACTION_HH__
#define __CPFLOAT_PROP_SUBTRACTION_HH__

#include <cpfloat/cpfloat.hh>

namespace MPG { namespace CPFloat { namespace Prop {
/**
 * \brief Propagates: \f$ x - y = z \f$
 * \ingroup SetProp
 */
template <typename ViewLeft1, typename ViewLeft2, typename ViewRight>
class Subtraction : public Gecode::Propagator {
protected:
  /// Left relation of the contraint
  ViewLeft1 left1_;
  /// Left relation of the contraint
  ViewLeft2 left2_;
  /// Right relation of the constraint
  ViewRight right_;
public:
  /// Constructor for the propagator \f$ Subtraction(left1,left2,right) \f$
  Subtraction(Gecode::Home home, ViewLeft1 left1, ViewLeft2 left2,ViewRight right)
    : Gecode::Propagator(home), left1_(left1), left2_(left2), right_(right) {
    left1_.subscribe(home,*this, CPFloat::PC_CPFLOAT_BND);
    left2_.subscribe(home,*this, CPFloat::PC_CPFLOAT_BND);
    right_.subscribe(home,*this, CPFloat::PC_CPFLOAT_BND);
  }
  /// Propagator posting
  static Gecode::ExecStatus post(Gecode::Home home,
                                 ViewLeft1 left1, ViewLeft2 left2, ViewRight right) {
    /// \todo Can we do some processing here and decide to not to post
    /// the constraint?
    (void) new (home) Subtraction(home,left1,left2,right);
    return Gecode::ES_OK;
  }
  /// Propagator disposal
  virtual size_t dispose(Gecode::Space& home) {
    left1_.cancel(home,*this,CPFloat::PC_CPFLOAT_BND);
    left2_.cancel(home,*this,CPFloat::PC_CPFLOAT_BND);
    right_.cancel(home,*this,CPFloat::PC_CPFLOAT_BND);
    (void) Propagator::dispose(home);
    return sizeof(*this);
  }
  /// Copy constructor
  Subtraction(Gecode::Space& home, bool share, Subtraction& p)
    : Gecode::Propagator(home,share,p) {
    left1_.update(home,share,p.left1_);
    left2_.update(home,share,p.left2_);
    right_.update(home,share,p.right_);
  }
  /// Copy
  virtual Gecode::Propagator* copy(Gecode::Space& home, bool share) {
    return new (home) Subtraction(home,share,*this);
  }
  /// Cost
  virtual Gecode::PropCost cost(const Gecode::Space&,
                                const Gecode::ModEventDelta&) const {
    return Gecode::PropCost::binary(Gecode::PropCost::LO);
  }
  /// Main propagation algorithm
  virtual Gecode::ExecStatus propagate(Gecode::Space& home,
                                       const Gecode::ModEventDelta&)  {

    GECODE_ME_CHECK(right_.leq(home,left1_.lub()-left2_.glb()));
    GECODE_ME_CHECK(right_.geq(home,left1_.glb()-left2_.lub()));

    GECODE_ME_CHECK(left2_.leq(home,right_.lub()-left1_.glb()));
    GECODE_ME_CHECK(left2_.geq(home,right_.glb()-left1_.lub()));

    GECODE_ME_CHECK(left1_.leq(home,right_.lub()+left2_.lub()));
    GECODE_ME_CHECK(left1_.geq(home,right_.glb()+left2_.glb()));

    // Propagator subsumpiton
    if (left1_.assigned() && left2_.assigned() && right_.assigned())
      return home.ES_SUBSUMED(*this);

    return Gecode::ES_FIX;
  }
};
}}}
#endif
