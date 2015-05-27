/*
 * Copyright (c) 2015, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include <xmlserializer/Node.h>
#include <stack>

namespace core
{
namespace bindings
{
namespace xml
{

class RulesBinder
{
public:

    RulesBinder(core::criterion::internal::Criteria &criteria) :
        mCriteria(criteria)
    {
    }

    core::xml::binding::Node getBindings()
    {
        using namespace core::xml::binding;
        Node crit {
            "SelectionCriterionRule",
            Body {
                Attributes {
                    { "SelectionCriterion", makeBinder(mTmpCriterionChild.criterionName) },
                    { "MatchesWhen", makeBinder(mTmpCriterionChild.matchType) },
                    { "Value", makeBinder(mTmpCriterionChild.value) },
                },
                Nodes {},
                Routine {
                    [this] () {
                        auto criterion =
                            mCriteria.getCriterion(mTmpCriterionChild.criterionName);
                        if (criterion != nullptr) {
                            mRules.top()->addRule(
                                std::shared_ptr<CSelectionCriterionRule>{
                                    new CSelectionCriterionRule{criterion,
                                                    mTmpCriterionChild.matchType,
                                                    mTmpCriterionChild.value}
                                });
                        } else {
                            throw std::runtime_error("Invalid Criterion");
                        }
                    }
                }
            }
        };
        Node compound {
            "CompoundRule",
            Body {
                Routine {
                    [this] () {
                        mRules.emplace(std::shared_ptr<CCompoundRule>{new CCompoundRule()});
                    }
                },
                Attributes { { "Type", makeBinder(mMatchType) } },
                Nodes { crit },
                Routine {
                    [this] () {
                        mChildRule = mRules.top();
                        mChildRule->setType(mMatchType == "All");
                        mRules.pop();
                        if (mRules.empty()) {
                            mRootRule = mChildRule;
                        } else {
                            mRules.top()->addRule(mChildRule);
                        }
                    }
                }
            }
        };
        compound.second.getChilds().emplace("CompoundRule", compound.second);
        return compound;
    }

    std::shared_ptr<CCompoundRule> &getRule()
    {
        return mRootRule;
    }

private:

    std::stack<std::shared_ptr<CCompoundRule> > mRules;
    std::shared_ptr<CCompoundRule> mRootRule;
    std::shared_ptr<CCompoundRule> mChildRule;

    struct RawCriterion
    {
        std::string criterionName = "";
        std::string matchType = "";
        std::string value = "";
    };

    RawCriterion mTmpCriterionChild;

    core::criterion::internal::Criteria &mCriteria;
    std::string mMatchType;
};


} /** xml namespace */
} /** bindings namespace */
} /** core namespace */
