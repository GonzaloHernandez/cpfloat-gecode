#ifndef __CPFLOAT_PROP_BASE_ACOS_HH__
#define __CPFLOAT_PROP_BASE_ACOS_HH__

#include <cpfloat/cpfloat.hh>

namespace MPG { namespace CPFloat { namespace Prop {
/**
 * \brief Propagates: \f$ y = acos(x) \f$
 * \ingroup SetProp
 */
template <typename ViewLeft, typename ViewRight>
class Acos : public Gecode::Propagator {
protected:
  /// Left relation of the contraint
  ViewLeft left_;
  /// Right relation of the constraint
  ViewRight right_;
public:
  /// Constructor for the propagator \f$ acos(left,right) \f$
  Acos(Gecode::Home home, ViewLeft left, ViewRight right)
    : Gecode::Propagator(home), left_(left), right_(right) {
    left_.subscribe(home,*this, CPFloat::PC_CPFLOAT_BND);
    right_.subscribe(home,*this,CPFloat::PC_CPFLOAT_BND);
  }
  /// Propagator posting
  static Gecode::ExecStatus post(Gecode::Home home,
                                 ViewLeft left, ViewRight right) {
    /// \todo Can we do some processing here and decide to not to post
    /// the constraint?
    (void) new (home) Acos(home,left,right);
    return Gecode::ES_OK;
  }
  /// Propagator disposal
  virtual size_t dispose(Gecode::Space& home) {
    left_.cancel(home,*this,CPFloat::PC_CPFLOAT_BND);
    right_.cancel(home,*this,CPFloat::PC_CPFLOAT_BND);
    (void) Propagator::dispose(home);
    return sizeof(*this);
  }
  /// Copy constructor
  Acos(Gecode::Space& home, bool share, Acos& p)
    : Gecode::Propagator(home,share,p) {
    left_.update(home,share,p.left_);
    right_.update(home,share,p.right_);
  }
  /// Copy
  virtual Gecode::Propagator* copy(Gecode::Space& home, bool share) {
    return new (home) Acos(home,share,*this);
  }
  /// Cost
  virtual Gecode::PropCost cost(const Gecode::Space&,
                                const Gecode::ModEventDelta&) const {
    return Gecode::PropCost::binary(Gecode::PropCost::LO);
  }
  /// Main propagation algorithm
  virtual Gecode::ExecStatus propagate(Gecode::Space& home,
                                       const Gecode::ModEventDelta&)  {

    INTERVAL x,y;
    x.lo = left_.glb();
    x.hi = left_.lub();
    y.lo = right_.glb();
    y.hi = right_.lub();

    narrow_acos(&x,&y);

    GECODE_ME_CHECK(left_.leq(home,x.hi));
    GECODE_ME_CHECK(left_.geq(home,x.lo));

    GECODE_ME_CHECK(right_.leq(home,y.hi));
    GECODE_ME_CHECK(right_.geq(home,y.lo));

    // Propagator subsumpiton
    if (left_.assigned() && right_.assigned())
      return home.ES_SUBSUMED(*this);

    return Gecode::ES_FIX;
  }
};
}}}
#endif
