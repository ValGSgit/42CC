#ifndef AFORM_HPP
#define AFORM_HPP

#include <iostream>
#include <string>
#include <exception>

class Bureaucrat;

class AForm {
	private:
		const std::string _name;
		bool _isSigned;
		const int _gradeToSign;
		const int _gradeToExecute;
		std::string _target;
	
	public:
		AForm();
		AForm(const std::string &name, int gradeToSign, int gradeToExecute);
		AForm(const std::string &name, int gradeToSign, int gradeToExecute, const std::string &target);
		AForm(const AForm &src);
		AForm &operator=(const AForm &rhs);
		virtual ~AForm();

		std::string getName() const;
		bool getIsSigned() const;
		int getGradeToSign() const;
		int getGradeToExecute() const;
		std::string getTarget() const;

		void beSigned(const Bureaucrat &bureaucrat);
		
		// Pure virtual function makes AForm an abstract class
		virtual void execute(Bureaucrat const &executor) const = 0;
		
		// Helper function to check execution requirements
		void checkExecutionRequirements(Bureaucrat const &executor) const;

		class GradeTooHighException : public std::exception {
		public:
			virtual const char *what() const throw();
		};

		class GradeTooLowException : public std::exception {
		public:
			virtual const char *what() const throw();
		};
		
		class FormNotSignedException : public std::exception {
		public:
			virtual const char *what() const throw();
		};
};

std::ostream &operator<<(std::ostream &out, const AForm &form);

#endif
