#pragma once

#include <memory>
#include <vector>
#include "common/hash_util.h"
#include "parser/expression/abstract_expression.h"
#include "type/value.h"

namespace terrier {
namespace parser {
namespace expression {

/**
 * Represents a logical constant expression.
 */
class ConstantValueExpression : public AbstractExpression {
 public:
  /**
   * Instantiate a new constant value expression.
   * @param value value to be held
   */
  explicit ConstantValueExpression(const type::Value &value)
      : AbstractExpression(ExpressionType::VALUE_CONSTANT, value.GetType(),
                           std::vector<std::unique_ptr<AbstractExpression>>()),
        value_(value) {}

  hash_t Hash() const override {
    ExpressionType expr = GetExpressionType();
    return HashUtil::CombineHashes(HashUtil::Hash(&expr), value_.Hash());
  }

  bool operator==(const AbstractExpression &other) const override {
    if (GetExpressionType() != other.GetExpressionType()) {
      return false;
    }
    auto const &const_expr = dynamic_cast<const ConstantValueExpression &>(other);
    return value_ == const_expr.GetValue();
  }

  bool operator!=(const AbstractExpression &rhs) const override { return !(*this == rhs); }

  AbstractExpression *Copy() const override { return new ConstantValueExpression(GetValue()); }

  /**
   * Returns the constant value stored in this constant value expression.
   * @return the value of the constant
   */
  type::Value GetValue() const { return value_; }

 private:
  type::Value value_;
};

}  // namespace expression
}  // namespace parser
}  // namespace terrier