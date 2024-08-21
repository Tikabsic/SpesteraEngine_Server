using Microsoft.AspNetCore.Http;
using System.Diagnostics;
using Microsoft.AspNetCore.Mvc;
using Services.Interfaces;
using Services.DTO;
namespace SpesteraEngine_WebServer.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class AccountController : ControllerBase
    {
        private readonly ILoginService _loginService;
        private readonly IRegisterService _registerService;

        public AccountController(ILoginService loginService,
            IRegisterService registerService
            )
        {
            _loginService = loginService;
            _registerService = registerService;
        }

        [HttpPost("login")]
        public async Task<ActionResult> Login([FromBody] LoginRequestDTO dto)
        {
            var request = await _loginService.LoginToServer(dto);

            if (request)
            {
                var loginData = await _loginService.SendLoginData(dto.AccountName);
                return Ok(loginData);
            }
            return BadRequest("Wrong username or password.");
        }

        [HttpPost("register")]
        public async Task<ActionResult> Register([FromBody] RegisterRequestDTO dto)
        {
            var request = await _registerService.RegisterNewUser(dto);
            return Ok(request);
        }
    }
}
