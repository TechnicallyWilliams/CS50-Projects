using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Web.Http;

namespace FriendBoard.Controllers
{
    public class ReviewsController : ApiController
    {
        // GET: api/Reviews
        public IEnumerable<string> Get()
        {
            return new string[] { "value1", "value2" };
        }

        // GET: api/Reviews/5
        public string Get(int id)
        {
            return "value";
        }

        // POST: api/Reviews
        public void Post([FromBody]string value)
        {
        }

        // PUT: api/Reviews/5
        public void Put(int id, [FromBody]string value)
        {
        }

        // DELETE: api/Reviews/5
        public void Delete(int id)
        {
        }
    }
}
