using FluentValidation;
using DatabaseIntegration.Database;
using Services.DTO;

namespace Utils.Validator
{

    public class AccountRegistrationValidator : AbstractValidator<RegisterRequestDTO>
    {
        public AccountRegistrationValidator(AppDbContext dbContext)
        {
            RuleFor(a => a.AccountName)
                .MinimumLength(5)
                .MaximumLength(16);

            RuleFor(a => a.ConfirmationPassword)
                .Equal(a => a.Password);

            RuleFor(a => a.Password)
                .MinimumLength(6)
                .MaximumLength(24);

            RuleFor(a => a.Email)
                .EmailAddress();

            RuleFor(a => a.Email)
                .Custom((value, context) =>
                {
                    var emailToValidate = dbContext.Accounts.Any(u => u.Email == value);

                    if (emailToValidate == true)
                    {
                        context.AddFailure("Email", "Email already in use.");
                    }
                });

            RuleFor(a => a.AccountName)
                .Custom((value, context) =>
                {
                    var emailToValidate = dbContext.Accounts.Any(u => u.AccountName == value);

                    if (emailToValidate == true)
                    {
                        context.AddFailure("AccountName", "Name already in use.");
                    }

                });
        }
    }
}
