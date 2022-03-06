#include <gecode/search.hh>
#include <gecode/gist.hh>
#include <cpfloat/cpfloat.hh>
#include <cpfloat/expression.hh>

using namespace Gecode;
using namespace MPG;
using namespace MPG::CPFloat;

class K3BBellido : public Gecode::Space {
protected:
  CPFloatVarArray a_;
public:
  K3BBellido(void)
    : a_(*this, 9, -1e8, 1e8) {
    VarExpression z1(a_[0]), z2(a_[1]), z3(a_[2]), z4(a_[3]),
    z5(a_[4]), z6(a_[5]), z7(a_[6]), z8(a_[7]), z9(a_[8]);

    k3b(*this, ((z1-6)^2)+(z2^2)+(z3^2) == 104 );
    k3b(*this, (z4^2)+((z5-6)^2)+(z6^2) == 104 );
    k3b(*this, (z7^2)+((z8-12)^2)+((z9-6)^2) == 80 );
    k3b(*this, z1*(z4-6)+z5*(z2-6)+z3*z6 == 52 );
    k3b(*this, z1*(z7-6)+z8*(z2-12)+z9*(z3-6) == -64 );
    k3b(*this, z4*z7+z8*(z5-12)+z9*(z6-6)-6*z5 == -32 );
    k3b(*this, 2*(z2+z3-z6) == z4+z5+z7+z9-18 );
    k3b(*this, z1+z2+2*(z3+z4+z6-z7)+z8-z9 == 38 );
    k3b(*this, z1+z3+z5-z6+2*(z7-z8-z4) == -8 );

    // To verification with the best solution in COCONUT project
    // Founded last 2 solutions
    hc4(*this, z1 == 9.3916661681 );
    hc4(*this, z2 == 9.2476345419 );
    hc4(*this, z3 == 2.6415631705 );
    hc4(*this, z4 == 7.9626675077 );

    naive(*this,a_);
  }

  void print(std::ostream& os) const {
    os << "z1: " << a_[0] << std::endl <<
          "z2: " << a_[1] << std::endl <<
          "z3: " << a_[2] << std::endl <<
          "z4: " << a_[3] << std::endl <<
          "z5: " << a_[4] << std::endl <<
          "z6: " << a_[5] << std::endl <<
          "z7: " << a_[6] << std::endl <<
          "z8: " << a_[7] << std::endl <<
          "z9: " << a_[8] << std::endl;
  }

  K3BBellido(bool share, K3BBellido& sp)
    : Gecode::Space(share,sp) {
    a_.update(*this, share, sp.a_);
  }

  virtual Space* copy(bool share) {
    return new K3BBellido(share,*this);
  }
};

int main(int, char**) {
  K3BBellido* g = new K3BBellido();

  Gist::Print<K3BBellido> p("Solved for: Bellido");
  Gist::Options o;
  o.inspect.click(&p);
  Gist::dfs(g,o);
  delete g;

  return 0;
}
