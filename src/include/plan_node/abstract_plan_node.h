#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "catalog/schema.h"
#include "common/hash_util.h"
#include "common/json.h"
#include "plan_node_defs.h"

// TODO(Gus,Wen): Add equaility operator and hash function support for output_schema

namespace terrier::plan_node {

//===--------------------------------------------------------------------===//
// Abstract Plan Node
//===--------------------------------------------------------------------===//

class AbstractPlanNode {
 public:
  AbstractPlanNode(std::shared_ptr<catalog::Schema> output_schema) : output_schema_(std::move(output_schema)) {}

  // For Deserialization
  AbstractPlanNode() {}

  virtual ~AbstractPlanNode() {}

  //===--------------------------------------------------------------------===//
  // Children Helpers
  //===--------------------------------------------------------------------===//

  void AddChild(std::unique_ptr<AbstractPlanNode> &&child) { children_.emplace_back(std::move(child)); }

  const std::vector<std::unique_ptr<AbstractPlanNode>> &GetChildren() const { return children_; }

  size_t GetChildrenSize() const { return children_.size(); }

  const AbstractPlanNode *GetChild(uint32_t child_index) const {
    TERRIER_ASSERT(child_index < children_.size(),
                   "index into children of plan node should be less than number of children");
    return children_[child_index].get();
  }

  //===--------------------------------------------------------------------===//
  // Accessors
  //===--------------------------------------------------------------------===//

  // Each sub-class will have to implement this function to return their type
  // This is better than having to store redundant types in all the objects
  virtual PlanNodeType GetPlanNodeType() const { return PlanNodeType::ABSTRACTPLAN; }

  // Get the output schema for the plan node. The output schema contains information on columns of the output of
  // the plan node operator
  std::shared_ptr<catalog::Schema> GetOutputSchema() const { return output_schema_; }

  // Get the estimated cardinality of this plan
  int GetEstimatedCardinality() const { return estimated_cardinality_; }

  // FOR TESTING ONLY. This function should only be called during construction of plan (ConvertOpExpression) or
  // for tests.
  void SetEstimatedCardinality(int cardinality) { estimated_cardinality_ = cardinality; }

  //===--------------------------------------------------------------------===//
  // JSON Serialization/Deserialization
  //===--------------------------------------------------------------------===//

  /**
   * Return the current plan node in JSON format.
   * @return JSON representation of plan node
   */
  virtual nlohmann::json ToJson() const;

  /**
   * Populates the plan node with the information in the given JSON.
   * Undefined behavior occurs if the JSON has a different PlanNodeType.
   */
  virtual void FromJson(const nlohmann::json &json);

  //===--------------------------------------------------------------------===//
  // Utilities
  //===--------------------------------------------------------------------===//

  // TODO(Gus,Wen): Schema needs a copy function in order to copy the shared ptr
  virtual std::unique_ptr<AbstractPlanNode> Copy() const {
    return std::unique_ptr<AbstractPlanNode>(new AbstractPlanNode(output_schema_));
  }

  virtual common::hash_t Hash() const {
    common::hash_t hash = 0;
    for (auto &child : GetChildren()) {
      hash = common::HashUtil::CombineHashes(hash, child->Hash());
    }
    return hash;
  }

  virtual bool operator==(const AbstractPlanNode &rhs) const {
    auto num = GetChildren().size();
    if (num != rhs.GetChildren().size()) return false;
    for (unsigned int i = 0; i < num; i++) {
      if (*GetChild(i) != *(AbstractPlanNode *)rhs.GetChild(i)) return false;
    }
    return true;
  }

  virtual bool operator!=(const AbstractPlanNode &rhs) const { return !(*this == rhs); }

 private:
  // A plan node can have multiple children
  std::vector<std::unique_ptr<AbstractPlanNode>> children_;

  int estimated_cardinality_ = 500000;

  std::shared_ptr<catalog::Schema> output_schema_;

 private:
  DISALLOW_COPY_AND_MOVE(AbstractPlanNode);
};

class Equal {
 public:
  bool operator()(const std::shared_ptr<plan_node::AbstractPlanNode> &a,
                  const std::shared_ptr<plan_node::AbstractPlanNode> &b) const {
    return *a.get() == *b.get();
  }
};

class Hash {
 public:
  size_t operator()(const std::shared_ptr<plan_node::AbstractPlanNode> &plan) const {
    return static_cast<size_t>(plan->Hash());
  }
};

// JSON library interface. Do not modify.
inline void to_json(nlohmann::json &j, const AbstractPlanNode &plan_node) { j = plan_node.ToJson(); }  /* NOLINT */
inline void from_json(const nlohmann::json &j, AbstractPlanNode &plan_node) { plan_node.FromJson(j); } /* NOLINT */

std::unique_ptr<AbstractPlanNode> DeserializePlanNode(const nlohmann::json &json);

}  // namespace terrier::plan_node
