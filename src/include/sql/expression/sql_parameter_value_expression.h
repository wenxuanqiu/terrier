#pragma once
#include <memory>
#include <vector>
#include "sql/expression/sql_abstract_expression.h"
#include "type/type_id.h"

namespace terrier::sql {

/**
 * Represents a parameter's offset in an expression.
 */
class SqlParameterValueExpression : public SqlAbstractExpression {
 public:
  /**
   * Instantiates a new ParameterValueExpression with the given offset.
   * @param value_idx the offset of the parameter
   */
  explicit SqlParameterValueExpression(const uint32_t value_idx)
      : SqlAbstractExpression(parser::ExpressionType::VALUE_PARAMETER, type::TypeId::PARAMETER_OFFSET, {}),
        value_idx_(value_idx) {}

  std::unique_ptr<SqlAbstractExpression> Copy() const override {
    return std::make_unique<SqlParameterValueExpression>(*this);
  }

  /**
   * @return offset in the expression
   */
  uint32_t GetValueIdx() { return value_idx_; }

 private:
  // TODO(Tianyu): Can we get a better name for this?
  uint32_t value_idx_;
};

}  // namespace terrier::sql