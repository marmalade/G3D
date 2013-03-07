using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;
using System.Diagnostics;
using G3D.Converter.Image;
using System.Reflection;
using System.Xml.Linq;

namespace G3D.Converter.Converter {
    public interface IConvertData {
        string type { get; }
    }
    public interface IConvertItem {
        string name { get; }
        void Setup(ConverterMgr _mgr,IConvertItem _parent);
        void Process(object item);
    }
    public interface IConvertModule : IConvertItem {
        //Setup is called on adding the module to the manager
        //StartExport is called when this type of export is run
        void StartExport();
    }
    public abstract class ConvertItem<Parent,Element> : IConvertItem {
        public string name { get; private set; }
        protected ConverterMgr mgr;
        protected Parent parent;

        public ConvertItem(string _name) { name=_name; }
        public void Setup(ConverterMgr _mgr,IConvertItem _parent) {
            mgr=_mgr;
            parent=(Parent)_parent;
        }
        public void Process(object item) {
            Process((Element)item);
        }
        protected abstract void Process(Element item);
    }
    public enum GetterType {
        Fetch,
        Create,
        FetchOrDefault,
        CreateOnly,
        CreateOrDefault,
        CreateAsDefault,
    }
    public class ConverterMgr : IConvertItem,IDisposable {
        public enum MsgType {
            Message,
            Warning,
            Error,
        }
        public class ToolSettings {
            public string filename;
            public Dictionary<string,string> values=new Dictionary<string,string>();
        }

        public ImageMgr imageMgr=new ImageMgr();

        public Configure configure = new Configure();
        public Collada collada = new Collada();

        public event Action<ConverterMgr.MsgType,string> output;
        private Dictionary<string,string> directories=new Dictionary<string,string>();
        private Dictionary<string,string> extensions=new Dictionary<string,string>();
        private Dictionary<string,ToolSettings> tools=new Dictionary<string,ToolSettings>();
        private Dictionary<string,IConvertItem> handlers=new Dictionary<string,IConvertItem>();
        private Dictionary<string,Func<IModule>> modules=new Dictionary<string,Func<IModule>>();
        private Dictionary<string,IConvertData> data=new Dictionary<string,IConvertData>();
        private Dictionary<string,IConvertData> defaults=new Dictionary<string,IConvertData>();

        public string name { get; set; }
        public bool force=false;
        public string export="text";
        public string projectFilename="";

        public ConverterMgr() {
            imageMgr.Init();

            SetHandler(configure);
            SetHandler(collada);
            SetHandler(new ConfigureCollada(),configure);
            AddExtension(".xml","configure");
            AddExtension(".dae","collada");
        }
        public void Dispose() {
            imageMgr.Shutdown();
        }

        public void AddDirectory(string name,string value) { directories[name]=value; }
        public void AddExtension(string name,string value) { extensions[name]=value; }
        public void AddTool(string name,ToolSettings tool) { tools[name]=tool; }
        public void SetHandler(IConvertItem handler,IConvertItem parent=null) { handlers[handler.name]=handler; handler.Setup(this,parent); }
        public void SetHandler(IConvertItem handler,string parent) {
            if(!handlers.ContainsKey(parent))
                return;
            handlers[handler.name]=handler;
            handler.Setup(this,handlers[parent]);
        }
        public void SetModuleFactory(string name,Func<IModule> func) { modules[name]=func; }
        public IModule MakeModule(string name) {
            if(!modules.ContainsKey(name))
                return null;
            IModule module=modules[name]();
            module.type=name;
            return module;
        }

        public T GetData<T>(string name,GetterType type=GetterType.Fetch) where T : class,IConvertData,new() {
            if(data.ContainsKey(name)) {
                if(type==GetterType.CreateOnly)
                    return null;
                return data[name] as T;
            }
            if(type==GetterType.Create || type==GetterType.CreateOnly) {
                Message("Creating "+name+" as "+typeof(T).Name);
                data[name]=new T();
                return data[name] as T;
            }
            if(type==GetterType.FetchOrDefault || type==GetterType.CreateOrDefault) {
                if(defaults.ContainsKey(typeof(T).FullName))
                    return defaults[typeof(T).FullName] as T;
            }
            if(type==GetterType.CreateOrDefault || type==GetterType.CreateAsDefault) {
                Message("Creating "+name+" as "+typeof(T).Name);
                data[name]=new T();
                defaults[typeof(T).FullName]=data[name];
                return data[name] as T;
            }
            return null;
        }
        public void SetDefault<T>(T data) where T : class,IConvertData,new() {
            defaults[typeof(T).FullName]=data;
        }
        public IEnumerable<string> GetDataNames() { return data.Keys; }

        public string GetName(IConvertData item) {
            if(!data.ContainsValue(item))
                return null;
            return data.FirstOrDefault((p) => p.Value==item).Key;
        }


        public void Convert(string filename) {
            filename=MakeAbsolute(filename,Environment.CurrentDirectory+"/x");
            Message("Loading file: "+filename);

            LoadPlugins();

            if(!handlers.ContainsKey(export)) {
                Error("Failed to find module "+export);
                return;
            }
            IConvertModule module=handlers[export] as IConvertModule;
            if(module!=null)
                module.StartExport();

            name=Path.GetFileNameWithoutExtension(filename);
            LoadConfig(filename);            //load project.xml in root of data hierarchy
            LoadFile(filename);              //load the file
            ProcessAll("process");          //convert the data
            Process(export,null);   //write out the data
        }
        public bool LoadFile(string filename) {
            string ext=Path.GetExtension(filename);
            if(extensions.ContainsKey(ext))
                ext=extensions[ext];
            if(!handlers.ContainsKey(ext))
                return false;
            if(!File.Exists(filename))
                return false;
            Run(handlers[ext],filename);
            return true;
        }
        public void LoadConfig(string filename) {
            string dir=Path.GetDirectoryName(MakeAbsolute(filename,filename));

            while(dir!=null && dir!="") {
                filename=Path.Combine(dir,"project.xml");
                if(File.Exists(filename)) {
                    break;
                }
                filename=null;
                dir=Path.GetDirectoryName(dir);
            }
            if(filename == null) return;

            projectFilename=filename;
            LoadFile(filename);
        }
        private void Run(IConvertItem file,string filename) {
            Message("Loading file "+filename);
            string config = Path.ChangeExtension(filename,".xml");
            file.Process(filename);
            if(File.Exists(config) && config!=filename)
                configure.ConvertSection(config,file.name);
        }
        public bool Process(string section,object data) {
            if(!handlers.ContainsKey(section))
                return false;
            handlers[section].Process(data);
            return true;
        }
        public bool ProcessAll(string section) {
            IConvertData[] array=data.Values.ToArray();
            List<IConvertData> processed=new List<IConvertData>();
            while(array.Length>0) {
                foreach(IConvertData d in array) {
                    string type=section+"."+d.type;
                    if(!handlers.ContainsKey(type))
                        continue;
                    handlers[type].Process(d);
                }
                processed.AddRange(array);
                array=data.Values.Except(processed).ToArray();
            }
            return true;
        }

        public bool RunTool(string name,string set,Dictionary<string,string> args) {
            if(!tools.ContainsKey(name))
                return false;
            if(!tools[name].values.ContainsKey(set))
                return false;

            string argString=tools[name].values[set];
            foreach(KeyValuePair<string,string> pair in args)
                argString=argString.Replace("{"+pair.Key+"}",pair.Value);

            Process proc=new Process();
            proc.StartInfo.UseShellExecute=false;
            proc.StartInfo.CreateNoWindow=true;
            proc.StartInfo.RedirectStandardError=true;
            proc.StartInfo.RedirectStandardOutput=true;

            proc.StartInfo.FileName=Environment.ExpandEnvironmentVariables(tools[name].filename);
            proc.StartInfo.Arguments=argString;

            Message("Executing: "+proc.StartInfo.FileName+" "+argString);
            try {
                proc.Start();
                if(!proc.WaitForExit(1000*60*60)) {
                    Error("out of time running "+proc.StartInfo.FileName);
                    proc.Close();
                    return false;
                } else
                    proc.WaitForExit();
            } catch(Exception) {
                Error("failed to run "+proc.StartInfo.FileName);
                proc.Close();
                return false;
            }

            while(!proc.StandardOutput.EndOfStream)
                Message("stdout",proc.StandardOutput.ReadLine());
            while(!proc.StandardError.EndOfStream)
                Message("stderr",proc.StandardError.ReadLine());

            proc.Close();
            return true;
        }
        public string GetToolValue(string name,string value) {
            if(!tools.ContainsKey(name) || !tools[name].values.ContainsKey(value))
                return null;
            return tools[name].values[value];
        }

        public string MakeFilename(string key,string dirName,string ext,bool inRoot=false) {
            string[] split=key.Split(".".ToCharArray(),2);
            if(name!=null) {
                if(split.Length<2)
                    split=new string[] { name,name };
                else
                    split=new string[] { name,split[1] };
            }
            if(split.Length<2)
                split=new string[] { "temp","temp" };
            if(!directories.ContainsKey(dirName))
                directories[dirName]=MakeAbsolute(dirName,projectFilename);
            string file=inRoot?directories[dirName]:Path.Combine(directories[dirName],split[0]);
            if(!Directory.Exists(file))
                Directory.CreateDirectory(file);
            return Path.Combine(file,split[1]+ext);
        }
        public string MakeAbsolute(string file,string parent) {
            if(file==null || Path.IsPathRooted(file))
                return file;
            return Path.GetFullPath(Path.Combine(Path.GetDirectoryName(parent),file));
        }

        void LoadPlugins() {
            string config=Path.ChangeExtension(Assembly.GetEntryAssembly().Location,".xml");

            if(File.Exists(config)) {
                XDocument doc=XDocument.Load(config);

                foreach(XElement plugin in doc.Root.Elements("plugin")) {
                    string file=MakeAbsolute((string)plugin.Attribute("filename"),config);
                    LoadPlugin(file);
                }
            } else {
                string path=Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);
                foreach(string file in Directory.EnumerateFiles(path).Where((s) => Path.GetExtension(s) == ".dll"))
                    LoadPlugin(file);
            }
        }
        private void LoadPlugin(string file) {
            try {
                Assembly ass=Assembly.LoadFrom(file);
                foreach(Type t in ass.GetExportedTypes().Where((t) => t.GetInterface("IConvertModule")!=null && !t.IsAbstract)) {
                    IConvertItem module=(IConvertItem)Activator.CreateInstance(t);
                    SetHandler(module);
                }
            } catch(BadImageFormatException) { }
        }

        public void Message(string msg) {
            if(output!=null)
                output(MsgType.Message,msg);
        }
        public void Warning(string msg) {
            if(output!=null)
                output(MsgType.Warning,msg);
        }
        public void Error(string msg) {
            if(output!=null)
                output(MsgType.Error,msg);
        }
        public void Message(string loc,string msg) {
            if(output!=null)
                output(MsgType.Message,loc+": "+msg);
        }
        public void Warning(string loc,string msg) {
            if(output!=null)
                output(MsgType.Warning,loc+": "+msg);
        }
        public void Error(string loc,string msg) {
            if(output!=null)
                output(MsgType.Error,loc+": "+msg);
        }

        public void Setup(ConverterMgr _mgr,IConvertItem _parent) { }
        public void Process(object item) { }
    }
}
