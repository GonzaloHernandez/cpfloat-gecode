#include <gecode/search.hh>
#include <gecode/gist.hh>
#include <cpfloat/cpfloat.hh>
#include <cpfloat/expression.hh>

using namespace Gecode;
using namespace MPG;
using namespace MPG::CPFloat;

class K3BCollins : public Gecode::Space {
protected:
  CPFloatVar x_;
public:
  K3BCollins(void)
    : x_(*this, -1.0, 1.0) {
    VarExpression x(x_);

    k3b(*this, ((3.9852+7.2338*(x^4)) - 10.039*(x^2) - 1.17775*(x^6)) + (20.091*(x^3) - 8.8575*x - 11.177*(x^5)) * (1.0-(x^2)).sqrt() == 0.0);

    branch(*this,x_);
  }

  void print(std::ostream& os) const {
    os << x_ << std::endl;
  }

  K3BCollins(bool share, K3BCollins& sp)
    : Gecode::Space(share,sp) {
    x_.update(*this, share, sp.x_);
  }

  virtual Space* copy(bool share) {
    return new K3BCollins(share,*this);
  }
};

int main(int, char**) {
  K3BCollins* g = new K3BCollins();

  Gist::Print<K3BCollins> p("Solved for: sin(x)=cos(x)");
  Gist::Options o;
  o.inspect.click(&p);
  Gist::dfs(g,o);
  delete g;

  return 0;
}
