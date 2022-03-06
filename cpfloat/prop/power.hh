#ifndef __CPFLOAT_PROP_POWER_HH__
#define __CPFLOAT_PROP_POWER_HH__

#include <cpfloat/cpfloat.hh>

namespace MPG { namespace CPFloat { namespace Prop {
/**
 * \brief Propagates: \f$ x ^ e = y \f$
 * \ingroup SetProp
 */
template <typename ViewLeft, typename ViewRight>
class Power : public Gecode::Propagator {
protected:
  /// Left relation of the contraint
  ViewLeft left_;
  /// Exponent relation of the contraint
  int e_;
  /// Right relation of the constraint
  ViewRight right_;
public:
  /// Constructor for the propagator \f$ Power(left,e,right) \f$
  Power(Gecode::Home home, ViewLeft left, int e,ViewRight right)
    : Gecode::Propagator(home), left_(left), e_(e), right_(right) {
    left_.subscribe(home,*this, CPFloat::PC_CPFLOAT_BND);
    right_.subscribe(home,*this, CPFloat::PC_CPFLOAT_BND);
  }
  /// Propagator posting
  static Gecode::ExecStatus post(Gecode::Home home,
                                 ViewLeft left, int e, ViewRight right) {
    /// \todo Can we do some processing here and decide to not to post
    /// the constraint?
    (void) new (home) Power(home,left,e,right);
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
  Power(Gecode::Space& home, bool share, Power& p)
    : Gecode::Propagator(home,share,p) {
    left_.update(home,share,p.left_);
    e_ = p.e_;
    right_.update(home,share,p.right_);
  }
  /// Copy
  virtual Gecode::Propagator* copy(Gecode::Space& home, bool share) {
    return new (home) Power(home,share,*this);
  }
  /// Cost
  virtual Gecode::PropCost cost(const Gecode::Space&,
                                const Gecode::ModEventDelta&) const {
    return Gecode::PropCost::binary(Gecode::PropCost::LO);
  }
  /// Main propagation algorithm
  virtual Gecode::ExecStatus propagate(Gecode::Space& home,
                                       const Gecode::ModEventDelta&)  {

      cout << " *** Power::propagate *** " << left_ << " " << right_ << endl;

    INTERVAL x,y,z;
    x.lo = left_.glb();
    x.hi = left_.lub();
    y.lo = e_;
    y.hi = e_;
    z.lo = right_.glb();
    z.hi = right_.lub();

    narrow_pow_odd(&x,&y,&z);

    GECODE_ME_CHECK(left_.leq(home,x.hi));
    GECODE_ME_CHECK(left_.geq(home,x.lo));

    GECODE_ME_CHECK(right_.leq(home,z.hi));
    GECODE_ME_CHECK(right_.geq(home,z.lo));

    // Propagator subsumpiton
    if (left_.assigned() && right_.assigned())
      return home.ES_SUBSUMED(*this);

    return Gecode::ES_FIX;
  }
};
}}}
#endif
