#pragma once

#include <string>
#include <memory>
#include <utility>
#include <vector>
#include "parser/create_function_statement.h"
#include "parser/parser_defs.h"
#include "plan_node/abstract_plan_node.h"

namespace terrier {
namespace plan_node {
/**
 * Plan node for creating user defined functions
 */
class CreateFunctionPlanNode : public AbstractPlanNode {
 protected:
  /**
   * Builder for an create function plan node
   */
  class Builder : public AbstractPlanNode::Builder<Builder> {
   public:
    DISALLOW_COPY_AND_MOVE(Builder);

    /**
     * @param language the UDF language type
     * @return builder object
     */
    Builder &SetLanguage(parser::PLType language) {
      language_ = language;
      return *this;
    }

    /**
     * @param function_param_names Function parameters names passed to the UDF
     * @return builder object
     */
    Builder &SetFunctionParamNames(std::vector<std::string> &&function_param_names) {
      function_param_names_ = std::move(function_param_names);
      return *this;
    }

    /**
     * @param function_param_types Function parameter types passed to the UDF
     * @return builder object
     */
    Builder &SetFunctionParamTypes(std::vector<parser::Parameter::DataType> &&function_param_types) {
      function_param_types_ = std::move(function_param_types);
      return *this;
    }

    /**
     * @param function_body query string/function body of the UDF
     * @return builder object
     */
    Builder &SetColumnNames(std::vector<std::string> &&function_body) {
      function_body_ = std::move(function_body);
      return *this;
    }

    /**
     * @param is_replace indicates if the function definition needs to be replaced
     * @return builder object
     */
    Builder &SetIsReplace(bool is_replace) {
      is_replace_ = is_replace;
      return *this;
    }

    /**
     * @param function_name function name of the UDF
     * @return builder object
     */
    Builder &SetFunctionName(std::string function_name) {
      function_name_ = std::move(function_name);
      return *this;
    }

    /**
     * @param return_type return type of the UDF
     * @return builder object
     */
    Builder &SetReturnType(parser::Parameter::DataType return_type) {
      return_type_ = return_type;
      return *this;
    }

    /**
     * @param param_count number of parameter of UDF
     * @return builder object
     */
    Builder &SetParamCount(int param_count) {
      param_count_ = param_count;
      return *this;
    }

    /**
     * @param create_func_stmt the SQL CREATE FUNCTION statement
     * @return builder object
     */
    Builder &SetFromCreateFuncStatement(parser::CreateFunctionStatement *create_func_stmt) {
      language_ = create_func_stmt->GetPLType();
      function_body_ = create_func_stmt->GetFuncBody();
      is_replace_ = create_func_stmt->ShouldReplace();
      function_name_ = create_func_stmt->GetFuncName();

      for (const auto &col : create_func_stmt->GetFuncParameters()) {
        function_param_names_.push_back(col->GetParamName());
        param_count_++;
        function_param_types_.push_back(col->GetDataType());
      }

      auto ret_type_obj = *(create_func_stmt->GetFuncReturnType());
      return_type_ = ret_type_obj.GetDataType();
      return *this;
    }

    /**
     * Build the create function plan node
     * @return plan node
     */
    std::shared_ptr<CreateFunctionPlanNode> Build() {
      return std::shared_ptr<CreateFunctionPlanNode>(new CreateFunctionPlanNode(
          std::move(children_), std::move(output_schema_), estimated_cardinality_, language_,
          std::move(function_param_names_), std::move(function_param_types_), std::move(function_body_), is_replace_,
          std::move(function_name_), return_type_, param_count_));
    }

   protected:
    parser::PLType language_;
    std::vector<std::string> function_param_names_;
    std::vector<parser::Parameter::DataType> function_param_types_;
    std::vector<std::string> function_body_;
    bool is_replace_;
    std::string function_name_;
    parser::Parameter::DataType return_type_;
    int param_count_ = 0;
  };

  /**
   * @param children child plan nodes
   * @param output_schema Schema representing the structure of the output of this plan node
   * @param estimated_cardinality estimated cardinality of output of node
   * @param language the UDF language type
   * @param function_param_names Function parameters names passed to the UDF
   * @param function_param_types Function parameter types passed to the UDF
   * @param function_body query string/function body of the UDF
   * @param is_replace indicates if the function definition needs to be replaced
   * @param function_name function name of the UDF
   * @param return_type return type of the UDF
   * @param param_count number of parameter of UDF
   */
  CreateFunctionPlanNode(std::vector<std::unique_ptr<AbstractPlanNode>> &&children,
                         std::shared_ptr<OutputSchema> output_schema, uint32_t estimated_cardinality,
                         parser::PLType language, std::vector<std::string> &&function_param_names,
                         std::vector<parser::Parameter::DataType> &&function_param_types,
                         std::vector<std::string> &&function_body, bool is_replace, std::string function_name,
                         parser::Parameter::DataType return_type, int param_count)
      : AbstractPlanNode(std::move(children), std::move(output_schema), estimated_cardinality),
        language_(language),
        function_param_names_(std::move(function_param_names)),
        function_param_types_(std::move(function_param_types)),
        function_body_(std::move(function_body)),
        is_replace_(is_replace),
        function_name_(std::move(function_name)),
        return_type_(return_type),
        param_count_(param_count) {}

 public:
  /**
   * @return the type of this plan node
   */
  PlanNodeType GetPlanNodeType() const override { return PlanNodeType::CREATE_FUNC; }

  /**
   * @return name of the user defined function
   */
  std::string GetFunctionName() const { return function_name_; }

  /**
   * @return language type of the user defined function
   */
  parser::PLType GetUDFLanguage() const { return language_; }

  /**
   * @return body of the user defined function
   */
  std::vector<std::string> GetFunctionBody() const { return function_body_; }

  /**
   * @return parameter names of the user defined function
   */
  std::vector<std::string> GetFunctionParameterNames() const { return function_param_names_; }

  /**
   * @return parameter types of the user defined function
   */
  std::vector<parser::Parameter::DataType> GetFunctionParameterTypes() const { return function_param_types_; }

  /**
   * @return return type of the user defined function
   */
  parser::Parameter::DataType GetReturnType() const { return return_type_; }

  /**
   * @return whether the definition of the user defined function needs to be replaced
   */
  bool IsReplace() const { return is_replace_; }

  /**
   * @return number of parameters of the user defined function
   */
  int GetParamCount() const { return param_count_; }

  /**
   * @return the hashed value of this plan node
   */
  common::hash_t Hash() const override;

  bool operator==(const AbstractPlanNode &rhs) const override;
  bool operator!=(const AbstractPlanNode &rhs) const override { return !(*this == rhs); }

 private:
  // Indicates the UDF language type
  parser::PLType language_;

  // Function parameters names passed to the UDF
  std::vector<std::string> function_param_names_;

  // Function parameter types passed to the UDF
  std::vector<parser::Parameter::DataType> function_param_types_;

  // Query string/ function body of the UDF
  std::vector<std::string> function_body_;

  // Indicates if the function definition needs to be replaced
  bool is_replace_;

  // Function name of the UDF
  std::string function_name_;

  // Return type of the UDF
  parser::Parameter::DataType return_type_;

  // Number of parameters
  int param_count_ = 0;

 public:
  DISALLOW_COPY_AND_MOVE(CreateFunctionPlanNode);
};
}  // namespace plan_node
}  // namespace terrier
