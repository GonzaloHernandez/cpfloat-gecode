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

class HC4Bronstein : public Gecode::Space {
protected:
  CPFloatVarArray a_;
public:
  HC4Bronstein(void)
    : a_(*this, 3, -1000.0, 1000.0) {
    VarExpression x(a_[0]), y(a_[1]), z(a_[2]);

    hc4(*this, ((x^2) + (y^2) + (z^2)) - 36.0 == 0.0 );
    hc4(*this, (x + y) - z == 0.0 );
    hc4(*this, (x * y) + (z^2) - 1.0 == 0.0 );

    firstfail(*this,a_);
  }

  void print(std::ostream& os) const {
    os << "x: " << a_[0] << std::endl <<
          "y: " << a_[1] << std::endl <<
          "z: " << a_[2] << std::endl;
  }

  HC4Bronstein(bool share, HC4Bronstein& sp)
    : Gecode::Space(share,sp) {
    a_.update(*this, share, sp.a_);
  }

  virtual Space* copy(bool share) {
    return new HC4Bronstein(share,*this);
  }
};

int main(int, char**) {
  HC4Bronstein* g = new HC4Bronstein();

  Gist::Print<HC4Bronstein> p("Solved for: Bronstein");
  Gist::Options o;
  o.inspect.click(&p);
  Gist::dfs(g,o);
  delete g;

  return 0;
}
