using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using G3D.Converter.Converter;
using G3D.Converter.Image;

namespace G3D.Converter.Plugin {
    class ConfigureProject : ConfigureSection {
        public ConfigureProject() : base("configure.project") { }
        public ConfigureProject(string name) : base(name) { }
        protected void ReadDirectories(XElement root) {
            foreach(XElement xDir in root.Elements("directory")) {
                string name=(string)xDir.Attribute("name");
                if(name==null)
                    mgr.Error(parent.GetLocation(xDir),"cannot find name");

                string dir=(string)xDir.Attribute("path");
                if(dir==null)
                    mgr.Error(parent.GetLocation(xDir),"cannot find value");

                mgr.AddDirectory(name,mgr.MakeAbsolute(dir,parent.filename));
            }
        }
        protected void ReadTools(XElement root) {
            foreach(XElement xTool in root.Elements("tool")) {
                string name=(string)xTool.Attribute("name");
                if(name==null)
                    mgr.Error(parent.GetLocation(xTool),"cannot find name");

                ConverterMgr.ToolSettings tool=new ConverterMgr.ToolSettings();
                tool.filename=(string)xTool.Attribute("filename");
                if(tool.filename==null)
                    mgr.Error(parent.GetLocation(xTool),"cannot find filename");

                foreach(XAttribute attr in xTool.Attributes())
                    if(attr.Name.LocalName!="name" && attr.Name.LocalName!="filename")
                        tool.values[attr.Name.LocalName]=attr.Value;

                mgr.AddTool(name,tool);
            }
        }
        protected void ReadModelSettings(XElement root) {
            XElement xSet=root.Element("model");
            if(xSet==null) return;

            ModelSettings settings=mgr.GetData<ModelSettings>("model.settings",GetterType.Create);
            settings.MakeDefault();

            ConfigureModel.ProcessSettings(settings,xSet);
        }
        protected void ReadTextureSettings(XElement root) {
            XElement xSet=root.Element("texture");
            if(xSet==null) return;

            TextureSettings settings=mgr.GetData<TextureSettings>("texture.settings",GetterType.Create);

            foreach(XElement child in xSet.Elements("format"))
                settings.formatChoices.Add(ConfigureTexture.LoadFormat(child));
        }
        protected void ReadNodeSettings(XElement root) {
            XElement xSet=root.Element("node");
            if(xSet==null) return;

            NodeSettings settings=mgr.GetData<NodeSettings>("node.settings",GetterType.Create);

            string scene=(string)xSet.Attribute("defaultScene");
            if(scene!=null)
                settings.defaultScene=scene;

            foreach(XElement tol in xSet.Elements("tolerance"))
                settings.toleranceMap[(string)tol.Attribute("name")]=(float)tol.Attribute("value");
        }
        protected void ReadInclude(XElement item) {
            string root = parent.filename;
            foreach(XElement xTool in item.Elements("include")) {
                string name=(string)xTool.Attribute("filename");
                if(name==null)
                    continue;
                name=mgr.MakeAbsolute(name,root);

                string oldName=mgr.name;
                mgr.name=Path.GetFileNameWithoutExtension(name);
                mgr.LoadFile(name);
                mgr.name=oldName;
            }
        }
        protected override void Process(XElement root) {
            ReadDirectories(root);
            ReadTools(root);
            ReadModelSettings(root);
            ReadTextureSettings(root);
            ReadNodeSettings(root);
            ReadInclude(root);
        }
    }
    class ConfigureLevel : ConfigureProject {
        public ConfigureLevel() : base("configure.level") { }
        protected override void Process(XElement root) {
            string name=(string)root.Attribute("name");
            if(name!=null)
                mgr.name=name;

            ReadModelSettings(root);
            ReadTextureSettings(root);
            ReadNodeSettings(root);
            ReadInclude(root);

            mgr.collada.nodeName=mgr.name;
            foreach(XElement child in root.Elements("gameobject"))
                mgr.Process("configure.gameobject",child);
        }
    }
    public abstract class BaseModule : IConvertModule {
        public string name { get; private set; }
        protected ConverterMgr mgr;

        public BaseModule(string _name) { name=_name; }
        public void Setup(ConverterMgr _mgr,IConvertItem _parent) {
            mgr=_mgr;
            Setup();
        }
        public void Process(object item) {
            Process();
        }
        protected abstract void Process();
        protected virtual void Setup() {
            ModelSettings model=mgr.GetData<ModelSettings>("model.settings",GetterType.Create);
            model.MakeDefault();
            model.outputTypes=Model.IndexType.Lines|Model.IndexType.LineStrips|Model.IndexType.Triangles|Model.IndexType.TriStrip|Model.IndexType.TriFans;

            NodeSettings node=mgr.GetData<NodeSettings>("node.settings",GetterType.Create);
            node.MakeDefault();

            // project description file
            mgr.SetHandler(new ConfigureProject(),mgr.configure);

            // level description file
            mgr.SetHandler(new ConfigureLevel(),mgr.configure);

            //texture handlers
            mgr.SetHandler(new ConfigureTexture(),mgr.configure);
            mgr.SetHandler(new TextureFile());
            mgr.SetHandler(new ProcessTexture());
            mgr.AddExtension(".bmp","texture");
            mgr.AddExtension(".dds","texture");
            mgr.AddExtension(".gif","texture");
            mgr.AddExtension(".jpg","texture");
            mgr.AddExtension(".jpeg","texture");
            mgr.AddExtension(".png","texture");
            mgr.AddExtension(".psd","texture");
            mgr.AddExtension(".psp","texture");
            mgr.AddExtension(".tga","texture");
            mgr.AddExtension(".tif","texture");
            mgr.AddExtension(".tiff","texture");

            //effect handlers
            mgr.SetHandler(new ConfigureEffect(),mgr.configure);
            mgr.SetHandler(new ProcessEffect());

            //collada handlers
            mgr.SetHandler(new ProcessModel());
            mgr.SetHandler(new ConfigureModel(),mgr.configure);
            mgr.SetHandler(new ColladaModel(),mgr.collada);
            mgr.SetHandler(new ColladaMaterial(),mgr.collada);
            mgr.SetHandler(new ColladaTexture(),mgr.collada);

            //node handlers
            mgr.SetHandler(new ColladaNode(),mgr.collada);
            mgr.SetHandler(new ConfigureGameObject(),mgr.configure);
            mgr.SetHandler(new ProcessGameObject());
            mgr.SetHandler(new ConfigureScene(),mgr.configure);
            mgr.SetHandler(new ProcessScene());

            //module handlers
            mgr.SetModuleFactory("model",() => new ModelModule());
            mgr.SetModuleFactory("dummy",() => new DummyModule());
            mgr.SetModuleFactory("camera",() => new CameraModule());
            mgr.SetModuleFactory("frameM",() => new FrameMModule());
            mgr.SetModuleFactory("framePQS",() => new FramePQSModule());
            mgr.SetModuleFactory("framePRS",() => new FramePRSModule());
            mgr.SetModuleFactory("frameIdent",() => new FrameIdentModule());
            mgr.SetModuleFactory("scene.simple",() => new SimpleSceneModule());
        }
        public virtual void StartExport() {
        }
    }
    class ColladaElementsScene : ColladaSection {
        public ColladaElementsScene() : base("collada.scene") { }
        protected override void Process(XElement item) {
            item=item.Parent;
            XElement xLib=item.Element(parent.ns+"library_geometries");
            if(xLib!=null)
                foreach(XElement xGeom in xLib.Elements(parent.ns+"geometry"))
                    mgr.Process("collada.model",xGeom);

            xLib=item.Element(parent.ns+"library_materials");
            if(xLib!=null)
                foreach(XElement xMat in xLib.Elements(parent.ns+"material"))
                    mgr.Process("collada.material",xMat);
        }
    }
    public class TextModule : BaseModule {
        public List<string> lines=new List<string>();

        public TextModule() : base("text") { }
        public TextModule(string name) : base(name) { }
        public override void StartExport()
        {
 	        base.StartExport();

            mgr.SetHandler(new ColladaElementsScene(),mgr.collada);

            mgr.SetHandler(new TextureTextWriter(),this);
            mgr.SetHandler(new EffectTextWriter(),this);
            mgr.SetHandler(new ModelTextWriter(),this);
            mgr.SetHandler(new MaterialTextWriter(),this);
        }
        protected override void Process() {
            TextFiles<Texture>("texture");
            TextOneFile<Effect>("effect");
            TextFiles<Model>("model");
            TextOneFile<Material>("material");
        }
        protected void TextFiles<T>(string type) where T : class,IConvertData,new() {
            foreach(string key in mgr.GetDataNames()) {
                T data=mgr.GetData<T>(key);
                if(data==null) continue;

                string filename=mgr.MakeFilename(key,"out",".g3d."+type);

                mgr.Message("Writing "+type+" file: "+filename);

                lines.Clear();
                mgr.Process("write."+type,data);
                File.WriteAllLines(filename,lines);
            }
        }
        protected void TextOneFile<T>(string type) where T : class,IConvertData,new() {
            string filename=mgr.MakeFilename(type,"out",".g3d."+type);
            mgr.Message("Writing "+type+" file: "+filename);

            lines.Clear();
            foreach(string key in mgr.GetDataNames()) {
                T data=mgr.GetData<T>(key);
                if(data==null) continue;

                mgr.Process("write."+type,data);
            }
            File.WriteAllLines(filename,lines);
        }
    }
    public class BinaryModule : BaseModule {
        public Binary binary=new Binary();

        public BinaryModule() : base("binary") { }
        public BinaryModule(string name) : base(name) { }
        public override void StartExport() {
            base.StartExport();

            mgr.SetHandler(new ColladaElementsScene(),mgr.collada);

            mgr.SetHandler(new TextureBinaryWriter(),this);
            mgr.SetHandler(new EffectBinaryWriter(),this);
            mgr.SetHandler(new ModelBinaryWriter(),this);
            mgr.SetHandler(new MaterialBinaryWriter(),this);
        }
        protected override void Process() {
            string filename=mgr.MakeFilename(mgr.name,"binary",".g3d",true);
            binary.OpenWad(filename);
            mgr.ProcessAll("write");
            binary.CloseWad();
        }
    }
    class ColladaHierScene : ColladaSection {
        public ColladaHierScene() : base("collada.scene") { }
        protected override void Process(XElement item) {
            item = item.Element(parent.ns+"instance_visual_scene");
            if(item==null) return;

            item=parent.GetFromLib(item,"url","visual_scene");
            if(item==null) return;

            parent.nodeName=mgr.name;
            mgr.Process("collada.node",item);
        }
    }
    public class HierarchyModule : BinaryModule {
        public HierarchyModule() : base("hierarchy") { }
        public override void StartExport() {
            base.StartExport();

            mgr.SetHandler(new ColladaHierScene(),mgr.collada);
            mgr.SetHandler(new GameObjectBinaryWriter(),this);
            mgr.SetHandler(new SceneBinaryWriter(),this);
        }
    }
    public class TextHierModule : TextModule {
        public TextHierModule() : base("texthier") { }
        public override void StartExport() {
            base.StartExport();

            mgr.SetHandler(new ColladaHierScene(),mgr.collada);
            mgr.SetHandler(new NodeTextWriter(),this);
        }
        protected override void Process() {
            base.Process();
            TextOneFile<Node>("node");
        }
    }
}
