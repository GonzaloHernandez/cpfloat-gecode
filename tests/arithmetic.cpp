#include <gecode/search.hh>
#include <gecode/gist.hh>
#include <cpfloat/cpfloat.hh>

using namespace Gecode;
using namespace MPG;
using namespace MPG::CPFloat;

class Arithmetic : public Gecode::Space {
protected:
  CPFloatVar x;
  CPFloatVar y;
  CPFloatVar z;
public:
  Arithmetic(void)  {
    x = CPFloatVar(*this, 0.9, 1.1);
    y = CPFloatVar(*this, 0.9, 1.1);
    z = CPFloatVar(*this, 1.9, 2.1);

    addition(*this,x,y,z);
    branch(*this,x);
    branch(*this,y);
    branch(*this,z);
  }

  void print(std::ostream& os) const {
    os << x << y <<  z << std::endl;
  }

  Arithmetic(bool share, Arithmetic& s)
    : Gecode::Space(share,s) {
    x.update(*this,share,s.x);
    y.update(*this,share,s.y);
    z.update(*this,share,s.z);
  }

  virtual Space* copy(bool share) {
    return new Arithmetic(share,*this);
  }
};

int main(int, char**) {
  Arithmetic* g = new Arithmetic();

  Gecode::DFS<Arithmetic> e(g);
  if (Arithmetic* s = e.next()) {
      s->print(std::cout);
      delete s;
  }
  delete g;

  return 0;
}
