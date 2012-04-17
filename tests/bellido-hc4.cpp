#include <gecode/search.hh>
#include <gecode/gist.hh>
#include <cpfloat/cpfloat.hh>
#include <cpfloat/expression.hh>

using namespace Gecode;
using namespace MPG;
using namespace MPG::CPFloat;

class HC4Bellido : public Gecode::Space {
protected:
  CPFloatVarArray a_;
public:
  HC4Bellido(void)
    : a_(*this, 9, -1e8, 1e8) {
    VarExpression z1(a_[0]), z2(a_[1]), z3(a_[2]), z4(a_[3]),
    z5(a_[4]), z6(a_[5]), z7(a_[6]), z8(a_[7]), z9(a_[8]);

    hc4(*this, ((z1-6)^2)+(z2^2)+(z3^2)-104==0 );
    hc4(*this, (z4^2)+((z5-6)^2)+(z6^2)-104==0 );
    hc4(*this, (z7^2)+((z8-12)^2)+((z9-6)^2)-80==0 );
    hc4(*this, z1*(z4-6)+z5*(z2-6)+z3*z6-52==0 );
    hc4(*this, z1*(z7-6)+z8*(z2-12)+z9*(z3-6)+64==0 );
    hc4(*this, z4*z7+z8*(z5-12)+z9*(z6-6)-6*z5+32==0 );
    hc4(*this, 2*z2+2*z3-2*z6-z4-z5-z7-z9+18==0 );
    hc4(*this, z1+z2+2*z3+2*z4+2*z6-2==0 );
    hc4(*this, z7+z8-z9-38==0 );
    hc4(*this, z1+z3+z5-z6+2*z7-2*z8-2*z4+8==0 );

    firstfail(*this,a_);
  }

  void print(std::ostream& os) const {
    os << a_ << std::endl;
  }

  HC4Bellido(bool share, HC4Bellido& sp)
    : Gecode::Space(share,sp) {
    a_.update(*this, share, sp.a_);
  }

  virtual Space* copy(bool share) {
    return new HC4Bellido(share,*this);
  }
};

int main(int, char**) {
  HC4Bellido* g = new HC4Bellido();

  Gist::Print<HC4Bellido> p("Solved for: Bellido");
  Gist::Options o;
  o.inspect.click(&p);
  Gist::dfs(g,o);
  delete g;

  return 0;
}
