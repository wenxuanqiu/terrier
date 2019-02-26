#pragma once

#include "abstract_plan_node.h"
#include "parser/expression/abstract_expression.h"

// TODO(Gus,Wen): Replace PerformBinding and VisitParameters
// TODO(Gus,Wen): Does this plan really output columns? This might be a special case

namespace terrier::plan_node {

class HashPlanNode : public AbstractPlanNode {
 public:
  typedef const parser::AbstractExpression HashKeyType;
  typedef std::unique_ptr<HashKeyType> HashKeyPtrType;

  HashPlanNode(catalog::Schema output_schema, std::vector<HashKeyPtrType> &hashkeys)
      : AbstractPlanNode(output_schema), hash_keys_(std::move(hashkeys)) {}

  inline PlanNodeType GetPlanNodeType() const override { return PlanNodeType::HASH; }

  const std::string GetInfo() const { return "HashPlanNode"; }

  inline const std::vector<HashKeyPtrType> &GetHashKeys() const { return hash_keys_; }

  std::unique_ptr<AbstractPlanNode> Copy() const override {
    std::vector<HashKeyPtrType> copied_hash_keys;
    for (const auto &key : hash_keys_) {
      copied_hash_keys.push_back(std::unique_ptr<HashKeyType>(key->Copy()));
    }
    return std::unique_ptr<AbstractPlanNode>(new HashPlanNode(GetOutputSchema(), copied_hash_keys));
  }

  common::hash_t Hash() const override;

  bool operator==(const AbstractPlanNode &rhs) const override;
  bool operator!=(const AbstractPlanNode &rhs) const override { return !(*this == rhs); }

 private:
  std::vector<HashKeyPtrType> hash_keys_;

 private:
  DISALLOW_COPY_AND_MOVE(HashPlanNode);
};

}  // namespace terrier::plan_node