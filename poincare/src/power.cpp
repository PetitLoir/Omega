extern "C" {
#include <assert.h>
#include <math.h>
}
#include <poincare/power.h>
#include <poincare/product.h>
#include "layout/exponent_layout.h"

float Power::approximate(Context& context) const {
  return powf(m_operands[0]->approximate(context), m_operands[1]->approximate(context));
}

Expression * Power::evaluate(Context& context) const {
  Expression * base = m_operands[0]->evaluate(context);
  Expression * exponent = m_operands[1]->evaluate(context);
  if (base == nullptr || exponent == nullptr) {
    return nullptr;
  }
  Expression * result = nullptr;
  if (base->type() == Expression::Type::Float && exponent->type() == Expression::Type::Float) {
    result = new Float(this->approximate(context));
  }
  if (base->type() == Expression::Type::Matrix && m_operands[1]->type() == Expression::Type::Integer) {
    result = evaluateOnMatrixAndInteger((Matrix *)base, (Integer *)(m_operands[1]), context);
  }
  delete base;
  delete exponent;
  return result;
}


Expression * Power::evaluateOnMatrixAndInteger(Matrix * m, Integer * i, Context& context) const {
  if (m->numberOfColumns() != m->numberOfRows()) {
    return nullptr;
  }
  // TODO: return identity matrix if i == 0
  int power = (int)(i->approximate(context));
  Expression * result = new Float(1);
  for (int k = 0; k < power; k++) {
    Expression * operands[2];
    operands[0] = result;
    operands[1] = m;
    Expression * product = new Product(operands, true);
    Expression * newResult = product->evaluate(context);
    delete result;
    result = newResult;
    delete product;
  }
  return result;
}

Expression::Type Power::type() const {
  return Expression::Type::Power;
}

Expression * Power::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  assert(numberOfOperands == 2);
  return new Power(newOperands, cloneOperands);
}

ExpressionLayout * Power::createLayout() const {
  return new ExponentLayout(m_operands[0]->createLayout(), m_operands[1]->createLayout());
}
