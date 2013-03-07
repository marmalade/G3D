using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Linq;
using G3D.Converter.Converter;

namespace G3D.Converter.Plugin {
    public class Effect : IConvertData {
        public enum Type {
            none, sampler, image,  //used by converter
            float1, float2, float3, float4,
            int1, int2, int3, int4,
            matrix2x2, matrix3x3, matrix4x4,
            tmatrix2x2, tmatrix3x3, tmatrix4x4,
            texture
        }
        public class Shader {
            public bool isHLSL;
            public bool isVertex=false;
            public string target=null;
            public string entry=null;
            public string source="";
            public string generate="";
            public Dictionary<string,byte[]> binary=new Dictionary<string,byte[]>();
            public Dictionary<string,string> defines=new Dictionary<string,string>();
        };
        public class Variable {
            public bool isStatic;
            public string name="";
            public Type type=Type.none;
            public string hint=null;
            public string aux=null;
        };

        public string name=null;
        public List<Shader> shaders=new List<Shader>();
        public List<Variable> variables=new List<Variable>();
        public string generate="";
        public Dictionary<string,byte[]> binary=new Dictionary<string,byte[]>();
        public string type { get { return "effect"; } }
    }
    public class ProcessEffect : ConvertItem<ConverterMgr,Effect> {
        public ProcessEffect() : base("process.effect") { }
        protected override void Process(Effect item) {
            foreach(Effect.Shader shader in item.shaders)
                if(shader.generate!=null && shader.generate!="program") {
                    foreach(string type in shader.generate.Split(",".ToCharArray(),StringSplitOptions.RemoveEmptyEntries))
                        if(type!="program")
                            Compile(shader,type);
                }
            if(item.generate!=null)
                foreach(string type in item.generate.Split(",".ToCharArray(),StringSplitOptions.RemoveEmptyEntries))
                    Compile(item,type);
        }

        private void WriteShader(Effect.Shader shader,string filename) {
            string output="";
            foreach(KeyValuePair<string,string> pair in shader.defines)
                output+=string.Format("#define {0} {1}\n",pair.Key,pair.Value);
            output+=shader.source;
            File.WriteAllText(filename,output);
        }
        private void Compile(Effect.Shader shader,string type) {
            Dictionary<string,string> args=new Dictionary<string,string>();
            args["temp"]=Path.GetTempPath();
            if(shader.entry!=null) args["entry"]=shader.entry;
            if(shader.target!=null) args["target"]=shader.target;
            args["in"]=Path.Combine(args["temp"],"in.source");
            args["out"]=Path.Combine(args["temp"],"out.binary");

            WriteShader(shader,args["in"]);

            if(shader.isVertex)
                mgr.RunTool(type,"vertex",args);
            else
                mgr.RunTool(type,"pixel",args);

            try {
                shader.binary[type]=File.ReadAllBytes(args["out"]);
            } catch(Exception) { }

            try {
                File.Delete(args["in"]);
            } catch(Exception) { }
            try {
                File.Delete(args["out"]);
            } catch(Exception) { }
        }
        private void Compile(Effect item,string type) {
            Dictionary<string,string> args=new Dictionary<string,string>();
            args["temp"]=Path.GetTempPath();
            args["vertex"]=Path.Combine(args["temp"],"v.source");
            args["pixel"]=Path.Combine(args["temp"],"p.source");
            args["out"]=Path.Combine(args["temp"],"out.bin");

            foreach(Effect.Shader shader in item.shaders)
                if(shader.generate.Contains("program"))
                    if(shader.isVertex)
                        WriteShader(shader,args["vertex"]);
                    else
                        WriteShader(shader,args["pixel"]);

            mgr.RunTool(type,"program",args);

            try {
                item.binary[type]=File.ReadAllBytes(args["out"]);
            } catch(Exception) { }

            try {
                File.Delete(args["vertex"]);
            } catch(Exception) { }
            try {
                File.Delete(args["pixel"]);
            } catch(Exception) { }
            try {
                File.Delete(args["out"]);
            } catch(Exception) { }
        }
    }
    public class ConfigureEffect : ConfigureSection {
        public ConfigureEffect() : base("configure.effect") { }
        protected override void Process(XElement item) {
            string name=(string)item.Attribute("name");
            if(name==null)
                name="base";
            name="effects."+name;

            Effect effect=mgr.GetData<Effect>(name,GetterType.CreateOnly);
            if(effect==null) return;
            effect.name=name;

            XAttribute def=item.Attribute("default");
            if(def!=null && (bool)def)
                mgr.SetDefault<Effect>(effect);

            foreach(XElement child in item.Elements()) {
                switch(child.Name.LocalName) {
                    case "glsl":
                        ReadShader(effect,child,false);
                        break;
                    case "hlsl":
                        ReadShader(effect,child,true);
                        break;
                    case "program":
                        effect.generate=(string)child.Attribute("generate");
                        if(effect.generate==null) effect.generate="";
                        break;
                    case "dynamic":
                        ReadVariable(effect,child,false);
                        break;
                    case "static":
                        ReadVariable(effect,child,true);
                        break;
                }
            }
        }

        private void ReadVariable(Effect effect,XElement child,bool isStatic) {
            Effect.Variable var=new Effect.Variable();
            var.isStatic=isStatic;
            var.name=(string)child.Attribute("name");
            var.aux=(string)child.Attribute("aux");
            var.hint=(string)child.Attribute("hint");
            if(child.Attribute("type")!=null)
                var.type=(Effect.Type)Enum.Parse(typeof(Effect.Type),(string)child.Attribute("type"));

            effect.variables.Add(var);
        }
        private void ReadShader(Effect effect,XElement child,bool isHLSL) {
            Effect.Shader shader=new Effect.Shader();
            shader.isHLSL=isHLSL;

            if(!isHLSL)
                shader.isVertex=((string)child.Attribute("type")=="vertex");
            else {
                shader.target=(string)child.Attribute("target");
                shader.entry=(string)child.Attribute("entry");
                shader.isVertex=(shader.target[0]=='v');
            }
            shader.generate=(string)child.Attribute("generate");
            if(shader.generate==null) shader.generate="";

            string filename=mgr.MakeAbsolute((string)child.Attribute("filename"),parent.filename);
            if(filename!=null) {
                if(!File.Exists(filename))
                    mgr.Error("failed to load shader: "+filename);
                else
                    shader.source=File.ReadAllText(filename);
            }
            for(var item=child.FirstNode;item!=null;item=item.NextNode) {
                if(item.NodeType==XmlNodeType.Text)
                    shader.source+=((XText)item).Value;
                else if(item.NodeType==XmlNodeType.Element) {
                    string name=(string)((XElement)item).Attribute("name");
                    string value=(string)((XElement)item).Attribute("value");
                    if(value==null) value="";
                    if(name!=null)
                        shader.defines[name]=value;
                }
            }

            effect.shaders.Add(shader);
        }
    }
    public class EffectTextWriter : ConvertItem<TextModule,Effect> {
        public EffectTextWriter() : base("write.effect") { }
        protected override void Process(Effect item) {
            string effectName=item.name.Split('.').Last();
            parent.lines.Add(string.Format("effect {0}",effectName));

            foreach(KeyValuePair<string,byte[]> pair in item.binary) {
                string ident=mgr.GetToolValue(pair.Key,"ident");

                parent.lines.Add(string.Format("program {0} inline {1}",ident==null?"0":ident,pair.Value.Length));
                WriteBinary(pair.Value);
            }
            foreach(Effect.Shader shader in item.shaders) {
                string type="vertex";
                if(!shader.isVertex) type="pixel";

                foreach(KeyValuePair<string,byte[]> pair in shader.binary) {
                    string ident=mgr.GetToolValue(pair.Key,"ident");

                    parent.lines.Add(string.Format("binary {0} {1} inline {2}",type,ident==null?"0":ident,pair.Value.Length));
                    WriteBinary(pair.Value);
                }

                string[] lines=shader.source.Replace("\r","").Split('\n');

                if(!shader.isHLSL)
                    parent.lines.Add(string.Format("glsl {0} inline {1}",type,lines.Length));
                else
                    parent.lines.Add(string.Format("hlsl {0} {1} inline {2}",shader.target==null?"":shader.target,shader.entry==null?"":shader.entry,lines.Length));

                parent.lines.AddRange(lines);

                foreach(KeyValuePair<string,string> pair in shader.defines)
                    parent.lines.Add(string.Format("define {0} {1} {2}",type,pair.Key,pair.Value));
            }
            foreach(Effect.Variable var in item.variables) {
                if(var.isStatic)
                    parent.lines.Add(string.Format("static {0} {1} {2}",var.name,var.type,var.aux==null?"":var.aux));
                else
                    parent.lines.Add(string.Format("dynamic {0} {1} {2}",var.name,var.type,var.aux==null?"":var.aux));
            }
        }
        private void WriteBinary(byte[] data) {
            int s1=4;
            int s2=s1*16;
            for(int i=0;i<data.Length;i+=s2) {
                string line="i ";
                for(int j=0;j<s2 && (i+j)<data.Length;j+=s1) {
                    for(int k=0;k<s1 && (i+j+k)<data.Length;k++)
                        line+=data[i+j+k].ToString("x2");
                    line+=" ";
                }
                parent.lines.Add(line);
            }
        }
    }
    internal class EffectBinaryWriter : ConvertItem<BinaryModule,Effect> {
        enum Flags : byte {
            Program=0,
            Shader=1,

            Vertex=2,
            Pixel=4,

            Binary=0,
            Source=8,

            HLSL=0,
            GLSL=16,
        };
        public EffectBinaryWriter() : base("write.effect") { }
        protected override void Process(Effect item) {
            parent.binary.OpenFile("EFCT",mgr.GetName(item));
            parent.binary.bw.Write((uint)item.variables.Count);
            parent.binary.bw.Write((uint)(item.binary.Count + item.shaders.Sum((s) => s.binary.Count) + item.shaders.Count));

            foreach(KeyValuePair<string,byte[]> pair in item.binary) {
                string ident=mgr.GetToolValue(pair.Key,"ident");
                uint identValue=0;
                if(ident!=null)
                    identValue=uint.Parse(ident,System.Globalization.NumberStyles.HexNumber);

                Flags flags=Flags.Program|Flags.Binary|Flags.GLSL;
                parent.binary.OpenFile("SHDR");
                parent.binary.bw.Write((byte)flags);
                parent.binary.bw.Write((uint)identValue);
                parent.binary.bw.Write((uint)pair.Value.Length);
                parent.binary.bw.Write(pair.Value);
                parent.binary.CloseFile();
            }
            foreach(Effect.Shader shader in item.shaders) {
                Flags flags=Flags.Shader|(shader.isVertex?Flags.Vertex:Flags.Pixel)|Flags.Binary|(shader.isHLSL?Flags.HLSL:Flags.GLSL);

                foreach(KeyValuePair<string,byte[]> pair in shader.binary) {
                    string ident=mgr.GetToolValue(pair.Key,"ident");
                    uint identValue=0;
                    if(ident!=null)
                        identValue=uint.Parse(ident,System.Globalization.NumberStyles.HexNumber);

                    parent.binary.OpenFile("SHDR");
                    parent.binary.bw.Write((byte)flags);
                    parent.binary.bw.Write((uint)identValue);
                    parent.binary.bw.Write((uint)pair.Value.Length);
                    parent.binary.bw.Write(pair.Value);
                    parent.binary.CloseFile();
                }

                flags|=Flags.Source;

                parent.binary.OpenFile("SHDR");
                parent.binary.bw.Write((byte)flags);
                if(shader.isHLSL) {
                    parent.binary.WriteString(shader.target==null?"":shader.target);
                    parent.binary.WriteString(shader.entry==null?"":shader.entry);
                }
                parent.binary.bw.Write((uint)shader.defines.Count);
                foreach(KeyValuePair<string,string> pair in shader.defines) {
                    parent.binary.WriteString(pair.Key);
                    parent.binary.WriteString(pair.Value);
                }
                parent.binary.WriteString(shader.source);
                parent.binary.CloseFile();
            }

            foreach(Effect.Variable var in item.variables) {
                switch(var.type) {
                    case Effect.Type.float1:
                        parent.binary.bw.Write((byte)(MaterialBinaryWriter.ParameterType.COMPONENT_1 | MaterialBinaryWriter.ParameterType.TYPE_FLOAT));
                        break;
                    case Effect.Type.float2:
                        parent.binary.bw.Write((byte)(MaterialBinaryWriter.ParameterType.COMPONENT_2 | MaterialBinaryWriter.ParameterType.TYPE_FLOAT));
                        break;
                    case Effect.Type.float3:
                        parent.binary.bw.Write((byte)(MaterialBinaryWriter.ParameterType.COMPONENT_3 | MaterialBinaryWriter.ParameterType.TYPE_FLOAT));
                        break;
                    case Effect.Type.float4:
                        parent.binary.bw.Write((byte)(MaterialBinaryWriter.ParameterType.COMPONENT_4 | MaterialBinaryWriter.ParameterType.TYPE_FLOAT));
                        break;
                    case Effect.Type.int1:
                        parent.binary.bw.Write((byte)(MaterialBinaryWriter.ParameterType.COMPONENT_1 | MaterialBinaryWriter.ParameterType.TYPE_INT));
                        break;
                    case Effect.Type.int2:
                        parent.binary.bw.Write((byte)(MaterialBinaryWriter.ParameterType.COMPONENT_2 | MaterialBinaryWriter.ParameterType.TYPE_INT));
                        break;
                    case Effect.Type.int3:
                        parent.binary.bw.Write((byte)(MaterialBinaryWriter.ParameterType.COMPONENT_3 | MaterialBinaryWriter.ParameterType.TYPE_INT));
                        break;
                    case Effect.Type.int4:
                        parent.binary.bw.Write((byte)(MaterialBinaryWriter.ParameterType.COMPONENT_4 | MaterialBinaryWriter.ParameterType.TYPE_INT));
                        break;
                    case Effect.Type.matrix2x2:
                        parent.binary.bw.Write((byte)(MaterialBinaryWriter.ParameterType.COMPONENT_2x2 | MaterialBinaryWriter.ParameterType.TYPE_MATRIX));
                        break;
                    case Effect.Type.tmatrix2x2:
                        parent.binary.bw.Write((byte)(MaterialBinaryWriter.ParameterType.COMPONENT_2x2 | MaterialBinaryWriter.ParameterType.TYPE_TRANSPOSE_MATRIX));
                        break;
                    case Effect.Type.matrix3x3:
                        parent.binary.bw.Write((byte)(MaterialBinaryWriter.ParameterType.COMPONENT_3x3 | MaterialBinaryWriter.ParameterType.TYPE_MATRIX));
                        break;
                    case Effect.Type.tmatrix3x3:
                        parent.binary.bw.Write((byte)(MaterialBinaryWriter.ParameterType.COMPONENT_3x3 | MaterialBinaryWriter.ParameterType.TYPE_TRANSPOSE_MATRIX));
                        break;
                    case Effect.Type.matrix4x4:
                        parent.binary.bw.Write((byte)(MaterialBinaryWriter.ParameterType.COMPONENT_4x4 | MaterialBinaryWriter.ParameterType.TYPE_MATRIX));
                        break;
                    case Effect.Type.tmatrix4x4:
                        parent.binary.bw.Write((byte)(MaterialBinaryWriter.ParameterType.COMPONENT_4x4 | MaterialBinaryWriter.ParameterType.TYPE_TRANSPOSE_MATRIX));
                        break;
                    case Effect.Type.texture:
                        parent.binary.bw.Write((byte)(MaterialBinaryWriter.ParameterType.COMPONENT_1 | MaterialBinaryWriter.ParameterType.TYPE_TEXTURE));
                        break;
                    default:
                        parent.binary.bw.Write((byte)0);
                        break;
                }
                parent.binary.WriteString(var.name);
                parent.binary.WriteString(var.aux==null?"":var.aux);
            }
            parent.binary.CloseFile();
        }
    }
}
