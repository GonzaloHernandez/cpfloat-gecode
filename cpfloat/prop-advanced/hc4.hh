#ifndef __CPFLOAT_PROP_ADVANCEDHC4_HH__
#define __CPFLOAT_PROP_ADVANCED_HC4_HH__

#include <cpfloat/cpfloat.hh>
#include <cpfloat/prop-advanced/expression.hh>

namespace MPG { namespace CPFloat { namespace Prop {

/**
 * \brief Propagates: \f$ exp = exp \f$
 * \ingroup SetProp
 */
class HC4 : public Gecode::Propagator {
protected:
  Constraint& cst_;
public:
  /// Constructor for the propagator \f$ HC4(cst) \f$
  HC4(Gecode::Home home, Constraint& cst)
    : Gecode::Propagator(home), cst_(cst) {
    cst_.subscribe(home,*this,CPFloat::PC_CPFLOAT_BND);
  }
  /// Propagator posting
  static Gecode::ExecStatus post(Gecode::Home home, Constraint& cst) {
    /// \todo Can we do some processing here and decide to not to post
    /// the constraint?
    (void) new (home) HC4(home,cst);
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
  HC4(Gecode::Space& home, bool share, HC4& p)
    : Gecode::Propagator(home,share,p), cst_(p.cst_.cloneToUpdate()) {
    cst_.update(home,share,p.cst_);
  }
  /// Copy
  virtual Gecode::Propagator* copy(Gecode::Space& home, bool share) {
    return new (home) HC4(home,share,*this);
  }
  /// Cost
  virtual Gecode::PropCost cost(const Gecode::Space&,
                                const Gecode::ModEventDelta&) const {
    return Gecode::PropCost::binary(Gecode::PropCost::LO);
  }
  /// Main propagation algorithm
  virtual Gecode::ExecStatus propagate(Gecode::Space& home,
                                       const Gecode::ModEventDelta&)  {
    cst_.evaluate();
    GECODE_ME_CHECK(cst_.propagate(home));
    if (cst_.assigned()) return home.ES_SUBSUMED(*this);

    return Gecode::ES_NOFIX;
  }
};
}}}
#endif
