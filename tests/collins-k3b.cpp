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
