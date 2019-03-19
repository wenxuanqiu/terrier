#pragma once

#include <memory>
#include <string>
#include <utility>
#include "catalog/catalog_defs.h"
#include "catalog/schema.h"
#include "parser/expression/abstract_expression.h"
#include "plan_node/abstract_plan_node.h"
#include "plan_node/abstract_scan_plan_node.h"

namespace terrier::plan_node {

/**
 * Plan node for sequanial table scan
 */
class SeqScanPlanNode : public AbstractScanPlanNode {
 public:
  /**
   * @param output_schema Schema representing the structure of the output of this plan node
   * @param predicate scan predicate
   * @param table_oid OID for table to scan
   * @param is_for_update flag for if scan is for an update
   * @param parallel flag for parallel scan
   */
  SeqScanPlanNode(std::shared_ptr<OutputSchema> output_schema, parser::AbstractExpression *predicate,
                  catalog::table_oid_t table_oid, bool is_for_update = false, bool parallel = false)
      : AbstractScanPlanNode(std::move(output_schema), predicate, is_for_update, parallel), table_oid_(table_oid) {}

  /**
   * @return the type of this plan node
   */
  PlanNodeType GetPlanNodeType() const override { return PlanNodeType::SEQSCAN; }

  /**
   * @return the OID for the table being scanned
   */
  catalog::table_oid_t GetTableOid() const { return table_oid_; }

  /**
   * @return the hashed value of this plan node
   */
  common::hash_t Hash() const override;

  bool operator==(const AbstractPlanNode &rhs) const override;
  bool operator!=(const AbstractPlanNode &rhs) const override { return !(*this == rhs); }

 private:
  // OID for table being scanned
  catalog::table_oid_t table_oid_;

 public:
  DISALLOW_COPY_AND_MOVE(SeqScanPlanNode);
};

}  // namespace terrier::plan_node
