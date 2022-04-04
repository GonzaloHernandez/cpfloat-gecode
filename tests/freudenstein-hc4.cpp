/*
 *  Authors:
 *     Gonzalo Hernández <gonzalohernandez@udenar.edu.co>
 *     Gustavo Gutierrez
 *
 *  Year of last major update
 *     2011
 * 
 *  This file is a complement of Gecode, the generic constraint
 *  development environment:
 *     http://www.gecode.org
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

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
    hc4(*this, (x1-29.0) + ((((x2+1.0)*x2)-14.0) * x2) == 0.0 );
    hc4(*this, (x1-13.0) + ((((5.0-x2)*x2)- 2.0) * x2) == 0.0 );

    randselection(*this,a_);
  }

  void print(std::ostream& os) const {
    os << "x1 = " << a_[0] << std::endl <<
          "x2 = " << a_[1] << std::endl;
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
