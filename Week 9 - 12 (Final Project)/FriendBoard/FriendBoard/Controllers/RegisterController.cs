using FriendBoard.Interfaces;
using FriendBoard.Models;
using FriendBoard.Repositories;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Web.Http;

namespace FriendBoard.Controllers
{
    public class RegisterController : ApiController
    {

        private IRegisterUsers _adapter;

        public RegisterController()
        {
            _adapter = new Register();
        }


        // GET: api/Register
        public IEnumerable<string> Get()
        {
            return new string[] { "value1", "value2" };
        }

        // GET: api/Register/5
        public string Get(int id)
        {
            return "value";
        }

        // POST: api/Register
        public void Post([FromBody]RegisterViewModel model)
        {
            _adapter.CreateUser(model);
        }

        // PUT: api/Register/5
        public void Put(int id, [FromBody]string value)
        {
        }

        // DELETE: api/Register/5
        public void Delete(int id)
        {
        }
    }
}
