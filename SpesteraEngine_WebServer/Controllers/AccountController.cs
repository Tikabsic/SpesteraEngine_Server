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
            var stopwatch = Stopwatch.StartNew();
            var request = await _loginService.LoginToServer(dto);

            if (request)
            {
                var characterList = await _loginService.SendGameCharactersList(dto.AccountName);

                stopwatch.Stop();
                var elapsedMilliseconds = stopwatch.ElapsedMilliseconds;
                Console.WriteLine($"Total time for operation: {elapsedMilliseconds} ms");

                return Ok(characterList);
            }
            return BadRequest("Wrong username or password.");
        }

        [HttpPost("register")]
        public async Task<ActionResult> Register([FromBody] RegisterRequestDTO dto)
        {
            var stopwatch = Stopwatch.StartNew();

            var request = await _registerService.RegisterNewUser(dto);

            stopwatch.Stop();
            var elapsedMilliseconds = stopwatch.ElapsedMilliseconds;
            Console.WriteLine($"Total time for operation: {elapsedMilliseconds} ms");
            return Ok(request);
        }
    }
}
