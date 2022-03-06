#ifndef __CPFLOAT_PROP_ADDITION_HH__
#define __CPFLOAT_PROP_ADDITION_HH__

#include <cpfloat/cpfloat.hh>

namespace MPG { namespace CPFloat { namespace Prop {
/**
 * \brief Propagates: \f$ x + y = z \f$
 * \ingroup SetProp
 */
template <typename ViewLeft1, typename ViewLeft2, typename ViewRight>
class Addition : public Gecode::Propagator {
protected:
  /// Left relation of the contraint
  ViewLeft1 left1_;
  /// Left relation of the contraint
  ViewLeft2 left2_;
  /// Right relation of the constraint
  ViewRight right_;
public:
  /// Constructor for the propagator \f$ Addition(left1,left2,right) \f$
  Addition(Gecode::Home home, ViewLeft1 left1, ViewLeft2 left2,ViewRight right)
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
    (void) new (home) Addition(home,left1,left2,right);
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
  Addition(Gecode::Space& home, bool share, Addition& p)
    : Gecode::Propagator(home,share,p) {
    left1_.update(home,share,p.left1_);
    left2_.update(home,share,p.left2_);
    right_.update(home,share,p.right_);
  }
  /// Copy
  virtual Gecode::Propagator* copy(Gecode::Space& home, bool share) {
    return new (home) Addition(home,share,*this);
  }
  /// Cost
  virtual Gecode::PropCost cost(const Gecode::Space&,
                                const Gecode::ModEventDelta&) const {
    return Gecode::PropCost::ternary(Gecode::PropCost::LO);
  }
  /// Main propagation algorithm
  virtual Gecode::ExecStatus propagate(Gecode::Space& home,
                                       const Gecode::ModEventDelta&)  {

    INTERVAL x,y,z;
    x.lo = left1_.glb();
    x.hi = left1_.lub();
    y.lo = left2_.glb();
    y.hi = left2_.lub();
    z.lo = right_.glb();
    z.hi = right_.lub();

    narrow_add(&x,&y,&z);

    GECODE_ME_CHECK(left1_.leq(home,x.hi));
    GECODE_ME_CHECK(left1_.geq(home,x.lo));

    GECODE_ME_CHECK(left2_.leq(home,y.hi));
    GECODE_ME_CHECK(left2_.geq(home,y.lo));

    GECODE_ME_CHECK(right_.leq(home,z.hi));
    GECODE_ME_CHECK(right_.geq(home,z.lo));

    // Propagator subsumpiton
    if (left1_.assigned() && left2_.assigned() && right_.assigned())
      return home.ES_SUBSUMED(*this);

    return Gecode::ES_FIX;
  }
};
}}}
#endif
