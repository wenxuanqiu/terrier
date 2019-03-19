#pragma once

#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <vector>
#include "common/hash_util.h"
#include "parser/expression/abstract_expression.h"
#include "parser/expression_defs.h"
#include "plan_node/abstract_plan_node.h"
#include "plan_node/plan_node_defs.h"

// TODO(Gus, Wen): Replace Perform Binding in AggregateTerm and AggregatePlanNode
// TODO(Gus, Wen): Replace VisitParameters
// TODO(Gus, Wen): figure out global aggregates

namespace terrier::plan_node {

/**
 * Plan node for aggregates
 */
class AggregatePlanNode : public AbstractPlanNode {
 public:
  /**
   * Information for each term being aggregated on
   */
  class AggregateTerm {
   public:
    /**
     *
     * @param aggregate_type Aggregate expression type
     * @param expr pointer to aggregate expression
     * @param distinct distinct flag
     */
    AggregateTerm(parser::ExpressionType aggregate_type, parser::AbstractExpression *expr, bool distinct)
        : aggregate_type_(aggregate_type), expression_(expr), distinct_(distinct) {}

    parser::ExpressionType aggregate_type_;  // Count, Sum, Min, Max, etc
    const parser::AbstractExpression *expression_;
    bool distinct_;  // Distinct flag for aggragate term (example COUNT(distinct order))
  };

  /**
   * @param output_schema Schema representing the structure of the output of this plan node
   * @param having_clause_predicate unique pointer to possible having clause predicate
   * @param aggregate_terms vector of aggregate terms for the aggregation
   * @param aggregate_strategy aggregation strategy to be used
   */
  AggregatePlanNode(std::shared_ptr<OutputSchema> output_schema,
                    std::unique_ptr<const parser::AbstractExpression> &&having_clause_predicate,
                    std::vector<AggregateTerm> aggregate_terms, AggregateStrategy aggregate_strategy)
      : AbstractPlanNode(std::move(output_schema)),
        having_clause_predicate_(std::move(having_clause_predicate)),
        aggregate_terms_(std::move(aggregate_terms)),
        aggregate_strategy_(aggregate_strategy) {}

  ~AggregatePlanNode() override {
    for (auto term : aggregate_terms_) {
      delete term.expression_;
    }
  }

  //===--------------------------------------------------------------------===//
  // ACCESSORS
  //===--------------------------------------------------------------------===//

  // TODO(Gus,Wen): Figure out how to represent global aggregates (example: count(*))
  /**
   * A global aggregate does not aggregate over specific columns, example: count(*)
   * @return true if aggregation is global
   */
  bool IsGlobal() const { return false; }

  /**
   * @return pointer to predicate for having clause
   */
  const parser::AbstractExpression *GetHavingClausePredicate() const { return having_clause_predicate_.get(); }

  /**
   * @return vector of aggregate terms
   */
  const std::vector<AggregateTerm> &GetAggregateTerms() const { return aggregate_terms_; }

  /**
   * @return aggregation strategy
   */
  AggregateStrategy GetAggregateStrategy() const { return aggregate_strategy_; }

  /**
   * @return the type of this plan node
   */
  PlanNodeType GetPlanNodeType() const override { return PlanNodeType::AGGREGATE; }

  /**
   * @return the hashed value of this plan node
   */
  common::hash_t Hash() const override;

  bool operator==(const AbstractPlanNode &rhs) const override;
  bool operator!=(const AbstractPlanNode &rhs) const override { return !(*this == rhs); }

 private:
  /**
   * @return true of two vectors of aggregate terms are equal
   */
  bool AreEqual(const std::vector<AggregatePlanNode::AggregateTerm> &A,
                const std::vector<AggregatePlanNode::AggregateTerm> &B) const;

  /**
   * @param agg_terms aggregate terms to be hashed
   * @return hash of agregate terms
   */
  common::hash_t HashAggregateTerms(const std::vector<AggregatePlanNode::AggregateTerm> &agg_terms) const;

 private:
  std::unique_ptr<const parser::AbstractExpression> having_clause_predicate_;
  const std::vector<AggregateTerm> aggregate_terms_;
  const AggregateStrategy aggregate_strategy_;

 public:
  DISALLOW_COPY_AND_MOVE(AggregatePlanNode);
};

}  // namespace terrier::plan_node
