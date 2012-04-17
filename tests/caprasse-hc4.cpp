#include <gecode/search.hh>
#include <gecode/gist.hh>
#include <cpfloat/cpfloat.hh>
#include <cpfloat/prop-advanced/expression.hh>

using namespace Gecode;
using namespace MPG;
using namespace MPG::CPFloat;

class HC4Caprasse : public Gecode::Space {
protected:
  CPFloatVarArray a_;
public:
  HC4Caprasse(void)
    : a_(*this, 4, -10.0, 10.0) {
    VarExpression x(a_[0]), y(a_[1]), z(a_[2]), t(a_[3]);

    hc4(*this, (((y^2) * z) + (x * y * t * 2.0)) - (x * 2.0) - z == 0.0);
    hc4(*this, ((x * 4.0 * (y^2) * z) - ((x^2) * x * z) - ((y^2) * 10.0) - (y * t * 10.0)) + ((x^2) * y * t * 4.0) + ((y^2) * y * t * 2.0) + ((x^2) * 4.0) + (x * z * 4.0) + 2.0 == 0.0 );

    firstfail(*this,a_);
  }

  void print(std::ostream& os) const {
    os << a_ << std::endl;
  }

  HC4Caprasse(bool share, HC4Caprasse& sp)
    : Gecode::Space(share,sp) {
    a_.update(*this, share, sp.a_);
  }

  virtual Space* copy(bool share) {
    return new HC4Caprasse(share,*this);
  }
};

int main(int, char**) {
  HC4Caprasse* g = new HC4Caprasse();

  Gist::Print<HC4Caprasse> p("Solved for: Caprasse");
  Gist::Options o;
  o.inspect.click(&p);
  Gist::dfs(g,o);
  delete g;

  return 0;
}
