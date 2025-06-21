#include "Form.hpp"
#include "Bureaucrat.hpp"

Form::Form() 
    : _name("Default Form"), 
      _isSigned(false), 
      _gradeToSign(150), 
      _gradeToExecute(150) {
    std::cout << "Form default constructor called" << std::endl;
}

Form::Form(const std::string &name, int gradeToSign, int gradeToExecute)
    : _name(name), 
      _isSigned(false), 
      _gradeToSign(gradeToSign), 
      _gradeToExecute(gradeToExecute) {
    std::cout << "Form parameterized constructor called" << std::endl;
    if (gradeToSign < 1 || gradeToExecute < 1)
        throw Form::GradeTooHighException();
    else if (gradeToSign > 150 || gradeToExecute > 150)
        throw Form::GradeTooLowException();
}

Form::Form(const Form &src)
    : _name(src._name),
      _isSigned(src._isSigned),
      _gradeToSign(src._gradeToSign),
      _gradeToExecute(src._gradeToExecute) {
    std::cout << "Form copy constructor called" << std::endl;
}

Form &Form::operator=(const Form &rhs) {
    std::cout << "Form copy assignment operator called" << std::endl;
    if (this != &rhs) {
        _isSigned = rhs._isSigned;
    }
    return *this;
}

Form::~Form() {
    std::cout << "Form destructor called" << std::endl;
}

std::string Form::getName() const {
    return _name;
}

bool Form::getIsSigned() const {
    return _isSigned;
}

int Form::getGradeToSign() const {
    return _gradeToSign;
}

int Form::getGradeToExecute() const {
    return _gradeToExecute;
}

void Form::beSigned(const Bureaucrat &bureaucrat) {
    if (bureaucrat.getGrade() <= _gradeToSign)
        _isSigned = true;
    else
        throw Form::GradeTooLowException();
}

const char *Form::GradeTooHighException::what() const throw() {
    return "Error: Form grade is too high (min grade is 1)";
}

const char *Form::GradeTooLowException::what() const throw() {
    return "Error: Form grade is too low (max grade is 150)";
}

std::ostream &operator<<(std::ostream &out, const Form &form) {
    out << "Form: " << form.getName()
        << ", Sign status: " << (form.getIsSigned() ? "Signed" : "Not signed")
        << ", Grade  to sign: " << form.getGradeToSign()
        << ", Grade  to execute: " << form.getGradeToExecute();
    return out;
}
