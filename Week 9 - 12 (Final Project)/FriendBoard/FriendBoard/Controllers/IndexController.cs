using FriendBoard.Interfaces;
using FriendBoard.Repositories;
using FriendBoard.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Web.Http;

namespace FriendBoard.Controllers
{
    public class IndexController : ApiController
    {
        // GET: api/Index
        /* public IEnumerable<string> Get()
         {
             return new string[] { "value1", "value2" };
         }*/

        // GET: api/Index/5
        private IDisplayIndex _repository;

        public IndexController()
        {
            _repository = new Index();
        }


        public IEnumerable<string> Get() //This creates an indexed array that gets sent to the front end
        {
            SogoViewModels.SogoIndex Sogo = _repository.GetIndexSogo();
            return new string[] { Sogo.Name, Sogo.Image, Sogo.Audio };
        }

        // POST: api/Index
        public void Post([FromBody]string value)
        {
        }

        // PUT: api/Index/5
        public void Put(int id, [FromBody]string value)
        {
        }

        // DELETE: api/Index/5
        public void Delete(int id)
        {
        }
    }
}
