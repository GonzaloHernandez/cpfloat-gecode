#include <gecode/search.hh>
#include <gecode/gist.hh>
#include <cpfloat/cpfloat.hh>
#include <cpfloat/expression.hh>

using namespace Gecode;
using namespace MPG;
using namespace MPG::CPFloat;

class HC4Freudenstein : public Gecode::Space {
protected:
  CPFloatVarArray a_;
public:
  HC4Freudenstein(void)
    : a_(*this, 2, -1e8, 1e8) {
    VarExpression x1(a_[0]), x2(a_[1]);

    //Freudenstein
    k3b(*this, (x1-29.0) + ((((x2+1.0)*x2)-14.0) * x2) == 0.0 );
    k3b(*this, (x1-13.0) + ((((5.0-x2)*x2)- 2.0) * x2) == 0.0 );

    firstfail(*this,a_);
  }

  void print(std::ostream& os) const {
    os << a_ << std::endl;
  }

  HC4Freudenstein(bool share, HC4Freudenstein& sp)
    : Gecode::Space(share,sp) {
    a_.update(*this, share, sp.a_);
  }

  virtual Space* copy(bool share) {
    return new HC4Freudenstein(share,*this);
  }
};

int main(int, char**) {
  HC4Freudenstein* g = new HC4Freudenstein();

  Gist::Print<HC4Freudenstein> p("Solved for: Freudenstein");
  Gist::Options o;
  o.inspect.click(&p);
  Gist::dfs(g,o);
  delete g;

  return 0;
}
