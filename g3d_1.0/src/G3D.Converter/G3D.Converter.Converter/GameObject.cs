using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using Microsoft.Xna.Framework;

namespace G3D.Converter.Converter {
    public interface IModule {
        string type { get; set; }
        void Load(XElement item);
        void Save(Binary binary);
    }
    public interface IGOBModule : IModule {
        string name { get; set; }
        bool enableOnInit { get; set; }
        void Setup(ConverterMgr _mgr,GameObject _parent);
    }
    public interface ISceneModule : IModule {
        void Setup(ConverterMgr _mgr,Scene _parent);
    }
    public abstract class GOBModule : IGOBModule {
        public string type { get; set; }
        public string name { get; set; }
        public bool enableOnInit { get; set; }

        protected ConverterMgr mgr;
        protected GameObject parent;

        public void Setup(ConverterMgr _mgr,GameObject _parent) {
            mgr=_mgr;
            parent=_parent;
        }

        public abstract void Load(XElement item);
        public abstract void Save(Binary binary);

        protected void LoadBase(XElement item,string defName,bool defEnable) {
            string xName=null;
            bool? xEnable=null;
            if(item!=null) {
                xName=(string)item.Attribute("name");
                xEnable=(bool?)item.Attribute("enableOnInit");
            }
            if(xName!=null)
                name=xName;
            else
                name=defName;
            if(xEnable!=null)
                enableOnInit=(bool)xEnable;
            else
                enableOnInit=defEnable;
        }
    }
    public abstract class SceneModule : ISceneModule {
        public string type { get; set; }
        protected ConverterMgr mgr;
        protected Scene parent;

        public void Setup(ConverterMgr _mgr,Scene _parent) {
            mgr=_mgr;
            parent=_parent;
        }

        public abstract void Load(XElement item);
        public abstract void Save(Binary binary);
    }
    public class GameObject : IConvertData {
        public string type { get { return "gameobject"; } }
        
        public List<IGOBModule> modules=new List<IGOBModule>();
        public List<string> children=new List<string>();

        //intermediate data
        public object nodeData=null;
        public bool gotFrame=false; //got a frame module
        public bool gotData=false;  //got a camera, model or dummy module (or equivalent)

        public void AddModule(ConverterMgr mgr,IGOBModule module) {
            module.Setup(mgr,this);
            modules.Add(module);
        }
    }
    public class Scene : IConvertData {
        public string type { get { return "scene"; } }
        public ISceneModule sceneData=null;
    }
}
