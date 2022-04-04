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

class HC4SinxCosx : public Gecode::Space {
protected:
  CPFloatVar x_;
public:
  HC4SinxCosx(void)
    : x_(*this, 0, 3.1416) {
    VarExpression x(x_);

    // sin(x) = cos(x)
    hc4(*this, x.sin() == x.cos() );

    branch(*this,x_);
  }

  void print(std::ostream& os) const {
    os << x_ << std::endl;
  }

  HC4SinxCosx(bool share, HC4SinxCosx& sp)
    : Gecode::Space(share,sp) {
    x_.update(*this, share, sp.x_);
  }

  virtual Space* copy(bool share) {
    return new HC4SinxCosx(share,*this);
  }
};

int main(int, char**) {
  HC4SinxCosx* g = new HC4SinxCosx();

  Gist::Print<HC4SinxCosx> p("Solved for: sin(x)=cos(x)");
  Gist::Options o;
  o.inspect.click(&p);
  Gist::dfs(g,o);
  delete g;

  return 0;
}
