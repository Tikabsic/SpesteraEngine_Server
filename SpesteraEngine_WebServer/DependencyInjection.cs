using DatabaseIntegration.Database;
using DatabaseIntegration.Interfaces;
using DatabaseIntegration.Repositories;
using Microsoft.EntityFrameworkCore;
using Services.Account;
using Services.AccountServices;
using Services.Interfaces;
using Utils.Hasher;

namespace SpesteraEngine_WebServer
{
    public static class DependencyInjection
    {
        public static IServiceCollection AddInjections(this IServiceCollection services, IConfiguration configuration)
        {
            //dbContext config and scope
            services.AddDbContext<AppDbContext>(options =>
                options.UseMySql(
                    configuration.GetConnectionString("ApplicationDatabase"),
                    new MySqlServerVersion(new Version(8, 4, 2)))
                .LogTo(Console.WriteLine, LogLevel.Information));
            //Repositories Scope
            services.AddScoped<IAccountRepository, AccountRepository>();

            //Scoped Services
            services.AddScoped<IRegisterService, RegisterService>();
            services.AddScoped<ILoginService, LoginService>();
            services.AddScoped<IPasswordHasher, PasswordHasher>();
            return services;
        }
    }
}