#include "AForm.hpp"
#include "Bureaucrat.hpp"

AForm::AForm() 
    : _name("Default Form"), 
      _isSigned(false), 
      _gradeToSign(150), 
      _gradeToExecute(150),
      _target("default") {
    std::cout << "AForm default constructor called" << std::endl;
}

AForm::AForm(const std::string &name, int gradeToSign, int gradeToExecute)
    : _name(name), 
      _isSigned(false), 
      _gradeToSign(gradeToSign), 
      _gradeToExecute(gradeToExecute),
      _target("default") {
    std::cout << "AForm parameterized constructor called" << std::endl;
    if (gradeToSign < 1 || gradeToExecute < 1)
        throw AForm::GradeTooHighException();
    else if (gradeToSign > 150 || gradeToExecute > 150)
        throw AForm::GradeTooLowException();
}

AForm::AForm(const std::string &name, int gradeToSign, int gradeToExecute, const std::string &target)
    : _name(name), 
      _isSigned(false), 
      _gradeToSign(gradeToSign), 
      _gradeToExecute(gradeToExecute),
      _target(target) {
    std::cout << "AForm parameterized constructor with target called" << std::endl;
    if (gradeToSign < 1 || gradeToExecute < 1)
        throw AForm::GradeTooHighException();
    else if (gradeToSign > 150 || gradeToExecute > 150)
        throw AForm::GradeTooLowException();
}

AForm::AForm(const AForm &src)
    : _name(src._name),
      _isSigned(src._isSigned),
      _gradeToSign(src._gradeToSign),
      _gradeToExecute(src._gradeToExecute),
      _target(src._target) {
    std::cout << "AForm copy constructor called" << std::endl;
}

AForm &AForm::operator=(const AForm &rhs) {
    std::cout << "AForm copy assignment operator called" << std::endl;
    if (this != &rhs) {
        _isSigned = rhs._isSigned;
        _target = rhs._target;
    }
    return *this;
}

AForm::~AForm() {
    std::cout << "AForm destructor called" << std::endl;
}

std::string AForm::getName() const {
    return _name;
}

bool AForm::getIsSigned() const {
    return _isSigned;
}

int AForm::getGradeToSign() const {
    return _gradeToSign;
}

int AForm::getGradeToExecute() const {
    return _gradeToExecute;
}

std::string AForm::getTarget() const {
    return _target;
}

void AForm::beSigned(const Bureaucrat &bureaucrat) {
    if (bureaucrat.getGrade() <= _gradeToSign)
        _isSigned = true;
    else
        throw AForm::GradeTooLowException();
}

void AForm::checkExecutionRequirements(Bureaucrat const &executor) const {
    if (!_isSigned)
        throw FormNotSignedException();
    if (executor.getGrade() > _gradeToExecute)
        throw GradeTooLowException();
}

const char *AForm::GradeTooHighException::what() const throw() {
    return "Error: Form grade is too high (min grade is 1)";
}

const char *AForm::GradeTooLowException::what() const throw() {
    return "Error: Form grade is too low (max grade is 150)";
}

const char *AForm::FormNotSignedException::what() const throw() {
    return "Error: Form is not signed";
}

std::ostream &operator<<(std::ostream &out, const AForm &form) {
    out << "Form: " << form.getName()
        << ", Sign status: " << (form.getIsSigned() ? "Signed" : "Not signed")
        << ", Grade to sign: " << form.getGradeToSign()
        << ", Grade to execute: " << form.getGradeToExecute()
        << ", Target: " << form.getTarget();
    return out;
}
