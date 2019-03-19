#pragma once

#include <memory>
#include <string>
#include <utility>
#include "plan_node/abstract_scan_plan_node.h"
#include "plan_node/output_schema.h"

namespace terrier::plan_node {
class CSVScanPlanNode : public AbstractScanPlanNode {
 public:
  /**
   * Constructs a sequential scan over a CSV file
   * @param file_name The file path
   * @param cols Information of the columns expected in each row of the CSV
   * @param delimiter The character that separates columns within a row
   * @param quote The character used to quote data (i.e., strings)
   * @param escape The character that should appear before any data characters
   * that match the quote character.
   * @param null_string the null string for the file
   */
  CSVScanPlanNode(std::shared_ptr<OutputSchema> output_schema, std::string file_name, char delimiter = ',',
                  char quote = '"', char escape = '"', std::string null_string = "")
      : AbstractScanPlanNode(std::move(output_schema), nullptr /* predicate */),
        file_name_(std::move(file_name)),
        delimiter_(delimiter),
        quote_(quote),
        escape_(escape),
        null_string_(std::move(null_string)) {}

  /**
   * @return the type of this plan node
   */
  PlanNodeType GetPlanNodeType() const override { return PlanNodeType::CSVSCAN; }

  /**
   * @return string representation of file name
   */
  const std::string &GetFileName() const { return file_name_; }

  /**
   * @return delimiter character for CSV
   */
  char GetDelimiterChar() const { return delimiter_; }

  /**
   * @return quote character for CSV
   */
  char GetQuoteChar() const { return quote_; }

  /**
   * @return escape character for CSV
   */
  char GetEscapeChar() const { return escape_; }

  /**
   * @return null string for CSV
   */
  const std::string &GetNullString() const { return null_string_; }

  /**
   * @return the hashed value of this plan node
   */
  common::hash_t Hash() const override;

  bool operator==(const AbstractPlanNode &rhs) const override;
  bool operator!=(const AbstractPlanNode &rhs) const override { return !(*this == rhs); }

 private:
  const std::string file_name_;
  char delimiter_;
  char quote_;
  char escape_;
  const std::string null_string_;

 public:
  DISALLOW_COPY_AND_MOVE(CSVScanPlanNode);
};

}  // namespace terrier::plan_node
