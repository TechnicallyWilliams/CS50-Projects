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
    public class SogoController : ApiController
    {
        private IHandleSogos _repository;

        public SogoController()
        {
            _repository = new Sogo();
        }

        // GET: api/Board
        /* public IEnumerable<string> Get() //it hits if the type is string
         {
            return new string[] { "value1", "value2" };
         } */


       
        // public async Task<IHttpActionResult> Register(RegisterViewModel data)
       /* public IHttpActionResult Get(int id)  //This is so you can return http code statuses
        {
            List<SogoViewModels.Sogo> SogoList = _repository.GetSogos(id);

            return Ok(SogoList);
        } */

        // GET: api/Board/5
        //[Authorize] "I think identity automatically correlates "Authroize" for those with admin roles
        public IEnumerable<SogoViewModels.Sogo> Get(int id)
        {
            List<SogoViewModels.Sogo> SogoList = _repository.GetSogos(id);

            return SogoList; //new string[]
        }



        // POST: api/Board
        public void Post([FromBody]SogoViewModels.Sogo sogo)
        {
            if (sogo.Name == null && sogo.Image == null && sogo.Audio == null)
                return;

            _repository.CreateSogo(sogo.Name, sogo.Image, sogo.Audio);
        }

        // PUT: api/Board/5
        public void Put(int id, [FromBody]SogoViewModels.Sogo sogo)
        {
            if (sogo.Name == null && sogo.Image == null && sogo.Audio == null)
                return;

            int audioId = id;

            _repository.UpdateSogo(sogo.Name, sogo.Image, sogo.Audio, audioId);
        }

        // DELETE: api/Board/5
        public void Delete(int id)
        {
            if (id == 0)
                return;

            _repository.DeleteSogo(id);
        }
    }
}
