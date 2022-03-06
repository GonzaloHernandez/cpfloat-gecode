#include <gecode/search.hh>
#include <gecode/gist.hh>
#include <cpfloat/cpfloat.hh>
#include <cpfloat/expression.hh>

using namespace Gecode;
using namespace MPG;
using namespace MPG::CPFloat;

class K3BChemistry : public Space {
protected:
  CPFloatVarArray a_;
public:
  K3BChemistry(void)
    : a_(*this, 5, 0, 100000000) {
    VarExpression x1(a_[0]), x2(a_[1]), x3(a_[2]), x4(a_[3]), x5(a_[4]);

    BoundType R = 10;
    BoundType R5 = 0.193;
    BoundType R6 = 0.002597/sqrt(40);
    BoundType R7 = 0.003448/sqrt(40);
    BoundType R8 = 0.00001799/40;
    BoundType R9 = 0.0002155/sqrt(40);
    BoundType R10 = 0.00003846/40;

    k3b(*this, 3*x5 == x1*(x2 + 1) );
    k3b(*this, x3*(x2*(2*x3+R7) + 2*R5*x3 + R6) ==  8*x5 );
    k3b(*this, x4*(R9*x2 + 2*x4) == 4*R*x5 );
    k3b(*this, x2*(2*x1 + x3*(x3+R7) + R8 + 2*R10*x2 + R9*x4) + x1 == R*x5 );
    k3b(*this, x2*(x1 + R10*x2 + x3*(x3+R7) + R8 + R9*x4) +x1 + x3*(R5*x3 + R6) + (x4^2) == 1 );

//    // To verification with the best solution in COCONUT project
//    k3b(*this, x1 == 0.0031141);
//    k3b(*this, x2 == 34.5979);

    firstfail(*this,a_);
  }

  void print(std::ostream& os) const {
    os << "x1: " << a_[0] << std::endl <<
          "x2: " << a_[1] << std::endl <<
          "x3: " << a_[2] << std::endl <<
          "x4: " << a_[3] << std::endl <<
          "x5: " << a_[4] << std::endl;
  }

  K3BChemistry(bool share, K3BChemistry& sp)
    : Gecode::Space(share,sp) {
    a_.update(*this, share, sp.a_);
  }

  virtual Space* copy(bool share) {
    return new K3BChemistry(share,*this);
  }
};

int main(int, char**) {
  K3BChemistry* g = new K3BChemistry();

  Gist::Print<K3BChemistry> p("Solved for: Chemistry");
  Gist::Options o;
  o.inspect.click(&p);
  Gist::dfs(g,o);
  delete g;

  return 0;
}
