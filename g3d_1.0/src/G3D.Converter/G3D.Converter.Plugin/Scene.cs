using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using G3D.Converter.Converter;

namespace G3D.Converter.Plugin {
    public class ConfigureScene : ConfigureSection {
        public ConfigureScene() : base("configure.scene") { }
        protected override void Process(XElement item) {
            NodeSettings settings=mgr.GetData<NodeSettings>("node.settings");

            string name=(string)item.Attribute("name");
            if(name==null)
                name=mgr.name;
            else
                name=mgr.name+"."+name;
            Scene scene=mgr.GetData<Scene>(name,GetterType.CreateAsDefault);
            if(scene==null) return;
            
            string type=(string)item.Attribute("type");
            if(type==null) type=settings.defaultScene;

            ISceneModule module=(ISceneModule)mgr.MakeModule("scene."+type);
            if(module==null) return;
            
            module.Setup(mgr,scene);
            scene.sceneData=module;
            module.Load(item);
        }
    }
    public class ProcessScene : ConvertItem<ConverterMgr,Scene> {
        public ProcessScene() : base("process.scene") { }

        protected override void Process(Scene scene) {
            if(scene.sceneData!=null) return;

            NodeSettings settings=mgr.GetData<NodeSettings>("node.settings");

            ISceneModule module=(ISceneModule)mgr.MakeModule("scene."+settings.defaultScene);
            if(module==null) return;

            module.Setup(mgr,scene);
            scene.sceneData=module;
            module.Load(null);
        }
    }
    public class SimpleSceneModule : SceneModule {
        public override void Load(XElement item) {
        }
        public override void Save(Binary binary) {
        }
    }
    internal class SceneBinaryWriter : ConvertItem<HierarchyModule,Scene> {
        public SceneBinaryWriter() : base("write.scene") { }
        protected override void Process(Scene item) {
            parent.binary.OpenFile("SCEN",mgr.GetName(item));

            parent.binary.WriteString(item.sceneData.type);
            item.sceneData.Save(parent.binary);
            parent.binary.CloseFile();
        }
    }
}
