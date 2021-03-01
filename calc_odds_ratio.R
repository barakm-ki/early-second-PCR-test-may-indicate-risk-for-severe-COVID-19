





positive_test_outcome_not_severe = 504
positive_test_outcome_severe = 59

negative_test_outcome_not_severe = 120
negative_test_outcome_severe = 4



d = data.frame(g=factor(1:2),s=c(negative_test_outcome_severe,negative_test_outcome_not_severe),f=c(positive_test_outcome_severe,positive_test_outcome_not_severe))
g = glm(s/(s+f)~g,weight=s+f , data=d,family="binomial")
summary(g)


summary_glm = exp(cbind(OR = coef(g) , confint.default(g,level=0.95)))

summary_glm
