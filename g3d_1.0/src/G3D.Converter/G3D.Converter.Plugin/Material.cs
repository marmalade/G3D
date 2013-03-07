using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using G3D.Converter.Converter;

namespace G3D.Converter.Plugin {
    class Material : IConvertData {
        public class Parameter {
            public string name;
            public Effect.Variable param=null;
            public float[] fData=null;
            public int[] iData=null;
            public string sData=null;
        }

        public string type { get { return "material"; } }

        public string name;
        public Effect effect=null;
        public List<Parameter> parameters=new List<Parameter>();
    }
    class ColladaMaterial : ColladaSection {
        public ColladaMaterial() : base("collada.material") { }
        protected override void Process(XElement item) {
            string name=mgr.name+"."+(string)item.Attribute("id");
            Dictionary<string,XElement> paramDict=new Dictionary<string,XElement>();

            XElement xInst=item.Element(parent.ns+"instance_effect");
            if(xInst==null) return;

            XElement xEffect=parent.GetFromLib(xInst,"url","effect");
            if(xEffect==null) return;

            XElement profileC=xEffect.Element(parent.ns+"profile_COMMON");
            XElement profileCG=xEffect.Element(parent.ns+"profile_CG");
            if(profileCG==null)
                profileCG=xEffect.Element(parent.ns+"profile_GLSL");
            if(profileC!=null) {
                XElement technique=null;
                XElement[] techniques=profileC.Elements(parent.ns+"technique").ToArray();
                if(techniques.Length>1)
                    technique=techniques.FirstOrDefault((t) => {
                        XAttribute sid=t.Attribute("sid");
                        if(sid==null) return false;
                        return string.Compare(sid.Value,"common",true)==0;
                    });
                if(techniques.Length>0 && technique==null)
                    technique=techniques[0];
                if(technique==null) {
                    mgr.Error(parent.GetLocation(profileC),"missing technique");
                    return;
                }

                XElement xDetails=technique.Element(parent.ns+"constant");
                if(xDetails==null)
                    xDetails=technique.Element(parent.ns+"lambert");
                if(xDetails==null)
                    xDetails=technique.Element(parent.ns+"phong");
                if(xDetails==null)
                    xDetails=technique.Element(parent.ns+"blinn");
                if(xDetails==null) {
                    mgr.Error(parent.GetLocation(technique),"missing constant/lambert/phong/blinn");
                    return;
                }

                Material mat=mgr.GetData<Material>(name,GetterType.CreateOnly);
                if(mat==null) return;
                mat.name=name;
                if(mat.effect==null)
                    mat.effect=mgr.GetData<Effect>("effects."+GetName(item),GetterType.FetchOrDefault);

                GetNewParams(paramDict,xEffect);
                GetNewParams(paramDict,profileC);
                GetNewParams(paramDict,technique);
                GetSetParams(paramDict,xInst);

                string[] ColOrTex=new string[] { "emission","ambient","diffuse","specular","reflective","transparent" };
                string[] ParamOrFloat=new string[] { "shininess","reflectivity","transparency","index_of_refraction" };
                foreach(XElement xParam in xDetails.Elements()) {
                    Material.Parameter param=null;
                    if(ColOrTex.Contains(xParam.Name.LocalName)) {
                        param=GetTextureParam(xParam,paramDict);
                        if(param==null)
                            param=GetFloatParam(xParam,"color",paramDict);
                    } else if(ParamOrFloat.Contains(xParam.Name.LocalName)) {
                        param=GetFloatParam(xParam,"float",paramDict);
                    }
                    if(param==null) continue;

                    Effect.Variable effectParam=mat.effect.variables.FirstOrDefault((p) => p.hint == param.name);
                    if(effectParam!=null) {
                        switch(effectParam.type) {
                            case Effect.Type.int1:
                            case Effect.Type.int2:
                            case Effect.Type.int3:
                            case Effect.Type.int4:
                                if(param.iData==null) continue;
                                break;
                            case Effect.Type.texture:
                                if(param.sData==null) continue;
                                break;
                            default:
                                if(param.fData==null) continue;
                                break;
                        }
                        param.name=effectParam.name;
                        param.param=effectParam;
                        mat.parameters.Add(param);
                    }
                }
            } else if(profileCG!=null) {
                XElement techniqueHint=xInst.Element(parent.ns+"technique_hint");
                XElement technique=null;
                XElement[] techniques=profileCG.Elements(parent.ns+"technique").ToArray();

                if(techniques.Length>1 && techniqueHint!=null && techniqueHint.Attribute("ref")!=null)
                    technique=techniques.FirstOrDefault((t) => {
                        XAttribute sid=t.Attribute("sid");
                        if(sid==null) return false;
                        return string.Compare(sid.Value,(string)techniqueHint.Attribute("ref"),true)==0;
                    });
                if(techniques.Length>0 && technique==null)
                    technique=techniques[0];
                if(technique==null) {
                    mgr.Error(parent.GetLocation(profileCG),"missing technique");
                    return;
                }

                Material mat=mgr.GetData<Material>(name,GetterType.CreateOnly);
                if(mat==null) return;

                mat.name=name;
                if(mat.effect==null) {
                    string effect=null;
                    XElement include=profileCG.Element(parent.ns+"include");
                    if(include!=null)
                        effect=(string)include.Attribute("sid");
                    if(effect==null)
                        effect=GetName(item);

                    mat.effect=mgr.GetData<Effect>("effects."+effect,GetterType.FetchOrDefault);
                }

                GetNewParams(paramDict,xEffect);
                GetNewParams(paramDict,profileCG);
                GetNewParams(paramDict,technique);
                GetSetParams(paramDict,xInst);
                GetSetParams(paramDict,technique);

                foreach(Effect.Variable effectParam in mat.effect.variables)
                    if(paramDict.ContainsKey(effectParam.name)) {
                        Material.Parameter param=GetParam(paramDict[effectParam.name],paramDict);
                        if(param!=null) {
                            param.param=effectParam;
                            mat.parameters.Add(param);
                        }
                    }
            }
        }
        private static string GetName(XElement item) {
            string effect=(string)item.Attribute("name");
            if(effect==null)
                effect=(string)item.Attribute("id");
            if(effect!=null) {
                string[] list=effect.Split('_');
                if(list.Length>1)
                    return list.Last();
            }
            return "base";
        }
        private void GetNewParams(Dictionary<string,XElement> paramDict,XElement effect) {
            foreach(XElement xParam in effect.Elements(parent.ns+"newparam")) {
                paramDict[(string)xParam.Attribute("sid")]=xParam;
            }
            foreach(XElement xParam in effect.Elements(parent.ns+"image")) {
                paramDict[(string)xParam.Attribute("id")]=xParam;
            }
        }
        private void GetSetParams(Dictionary<string,XElement> paramDict,XElement effInst) {
            foreach(XElement xParam in effInst.Elements(parent.ns+"setparam")) {
                string id=(string)xParam.Attribute("ref");

                paramDict[id]=xParam;
            }
        }
        private Material.Parameter GetTextureParam(XElement item,Dictionary<string,XElement> paramDict) {
            XElement text=item.Element(parent.ns+"texture");
            if(text==null) return null;

            string texture=(string)text.Attribute("texture");
            if(texture==null)
                return null;

            XElement sampler=paramDict[texture];
            XElement surface=null;

            if(sampler!=null)
                sampler=GetParamData(sampler);
            if(sampler!=null)
                sampler=sampler.Element(parent.ns+"source");
            if(sampler!=null)
                surface=paramDict[sampler.Value];
            if(surface!=null)
                surface=GetParamData(surface);
            if(surface!=null)
                surface=surface.Element(parent.ns+"init_from");
            if(surface!=null)
                texture=surface.Value;

            mgr.Process("collada.texture",texture);

            Material.Parameter param=new Material.Parameter();
            param.name=item.Name.LocalName;
            param.sData=mgr.name+"."+texture;
            return param;
        }
        private XElement GetParamData(XElement xParam) {
            return xParam.Elements().FirstOrDefault((x) => {
                string n=x.Name.LocalName;
                return n.StartsWith("bool") || n.StartsWith("int") || n.StartsWith("float") ||
                    n.StartsWith("surface") || n.StartsWith("sampler");
            });
        }
        private Material.Parameter GetFloatParam(XElement item,string element,Dictionary<string,XElement> paramDict) {
            Material.Parameter param=new Material.Parameter();

            XElement val=item.Element(parent.ns+element);
            XElement xParam=item.Element(parent.ns+"param");
            if(xParam!=null) {
                string pName=(string)xParam.Attribute("ref");
                if(pName==null)
                    mgr.Error(parent.GetLocation(xParam),"missing ref");
                else
                    val=paramDict[pName];
            }

            if(val==null)
                return null;

            param.name=item.Name.LocalName;
            param.fData=val.Value.Split(" \t\n".ToArray(),StringSplitOptions.RemoveEmptyEntries).Select((s) => float.Parse(s)).ToArray();
            return param;
        }
        private Material.Parameter GetParam(XElement xParam,Dictionary<string,XElement> paramDict) {
            Material.Parameter param=new Material.Parameter();
            param.name=(string)xParam.Attribute("ref");

            foreach(XElement val in xParam.Elements()) {
                string n=val.Name.LocalName;

                if(n.StartsWith("float"))
                    param.fData=val.Value.Split(" \t\n".ToArray(),StringSplitOptions.RemoveEmptyEntries).Select((s) => float.Parse(s)).ToArray();
                else if(n.StartsWith("int"))
                    param.iData=val.Value.Split(" \t\n".ToArray(),StringSplitOptions.RemoveEmptyEntries).Select((s) => int.Parse(s)).ToArray();
                else if(n.StartsWith("sampler")) {
                    XElement source=val.Element(parent.ns+"source");
                    if(source==null || !paramDict.ContainsKey(source.Value)) {
                        mgr.Error(parent.GetLocation(xParam),"cannot find surface");
                        return null;
                    }
                    param=GetParam(paramDict[source.Value],paramDict);
                    param.name=(string)xParam.Attribute("ref");
                    return param;
                } else if(n.StartsWith("surface")) {
                    XElement source=val.Element(parent.ns+"init_from");
                    if(source==null) {
                        mgr.Error(parent.GetLocation(xParam),"cannot find image");
                        return null;
                    }

                    string texture=source.Value;

                    mgr.Process("collada.texture",texture);
                    param.sData=mgr.name+"."+texture;
                    return param;
                } else
                    return null;
            }
            return param;
        }
    }
    internal class MaterialTextWriter : ConvertItem<TextModule,Material> {
        public MaterialTextWriter() : base("write.material") { }
        protected override void Process(Material item) {
            parent.lines.Add(string.Format("material {0} {1}",item.name.Split('.').Last(),item.effect.name.Split('.').Last()));

            foreach(Material.Parameter param in item.parameters) {
                string value="";
                if(param.sData!=null)
                    value=param.sData.Split('.').Last()+".g3d.texture";
                else if(param.fData!=null)
                    value=string.Join(" ",param.fData);
                else if(param.iData!=null)
                    value=string.Join(" ",param.iData);
                parent.lines.Add(string.Format("parameter {0} {1} {2}",param.name,param.param.type.ToString(),value));
            }
            parent.lines.Add("");
        }
    }
    internal class MaterialBinaryWriter : ConvertItem<BinaryModule,Material> {
        public enum ParameterType : byte {
            COMPONENT_1 = 0,
            COMPONENT_2 = 1,
            COMPONENT_3 = 2,
            COMPONENT_4 = 3,
            COMPONENT_2x2 = 1,
            COMPONENT_3x3 = 2,
            COMPONENT_4x4 = 3,
            COMPONENT_MASK = 3,

            TYPE_FLOAT = 0,
            TYPE_INT = 4,
            TYPE_MATRIX = 8,
            TYPE_TRANSPOSE_MATRIX = 12,
            TYPE_TEXTURE = 16,
            TYPE_MASK = 28,
        };
        public MaterialBinaryWriter() : base("write.material") { }
        protected override void Process(Material item) {
            parent.binary.OpenFile("MAT ",mgr.GetName(item));
            parent.binary.WriteReference(mgr.GetName(item.effect));
            parent.binary.bw.Write((uint)item.parameters.Count);

            foreach(Material.Parameter param in item.parameters) {
                parent.binary.WriteString(param.name);

                switch(param.param.type) {
                    case Effect.Type.float1:
                        parent.binary.bw.Write((byte)(ParameterType.COMPONENT_1 | ParameterType.TYPE_FLOAT));
                        parent.binary.bw.Write(param.fData[0]);
                        break;
                    case Effect.Type.float2:
                        parent.binary.bw.Write((byte)(ParameterType.COMPONENT_2 | ParameterType.TYPE_FLOAT));
                        parent.binary.bw.Write(param.fData[0]);
                        parent.binary.bw.Write(param.fData[1]);
                        break;
                    case Effect.Type.float3:
                        parent.binary.bw.Write((byte)(ParameterType.COMPONENT_3 | ParameterType.TYPE_FLOAT));
                        parent.binary.bw.Write(param.fData[0]);
                        parent.binary.bw.Write(param.fData[1]);
                        parent.binary.bw.Write(param.fData[2]);
                        break;
                    case Effect.Type.float4:
                        parent.binary.bw.Write((byte)(ParameterType.COMPONENT_4 | ParameterType.TYPE_FLOAT));
                        parent.binary.bw.Write(param.fData[0]);
                        parent.binary.bw.Write(param.fData[1]);
                        parent.binary.bw.Write(param.fData[2]);
                        parent.binary.bw.Write(param.fData[3]);
                        break;
                    case Effect.Type.int1:
                        parent.binary.bw.Write((byte)(ParameterType.COMPONENT_1 | ParameterType.TYPE_INT));
                        parent.binary.bw.Write(param.iData[0]);
                        break;
                    case Effect.Type.int2:
                        parent.binary.bw.Write((byte)(ParameterType.COMPONENT_2 | ParameterType.TYPE_INT));
                        parent.binary.bw.Write(param.iData[0]);
                        parent.binary.bw.Write(param.iData[1]);
                        break;
                    case Effect.Type.int3:
                        parent.binary.bw.Write((byte)(ParameterType.COMPONENT_3 | ParameterType.TYPE_INT));
                        parent.binary.bw.Write(param.iData[0]);
                        parent.binary.bw.Write(param.iData[1]);
                        parent.binary.bw.Write(param.iData[2]);
                        break;
                    case Effect.Type.int4:
                        parent.binary.bw.Write((byte)(ParameterType.COMPONENT_4 | ParameterType.TYPE_INT));
                        parent.binary.bw.Write(param.iData[0]);
                        parent.binary.bw.Write(param.iData[1]);
                        parent.binary.bw.Write(param.iData[2]);
                        parent.binary.bw.Write(param.iData[3]);
                        break;
                    case Effect.Type.matrix2x2:
                        parent.binary.bw.Write((byte)(ParameterType.COMPONENT_2x2 | ParameterType.TYPE_MATRIX));
                        parent.binary.bw.Write(param.fData[0]);
                        parent.binary.bw.Write(param.fData[1]);
                        parent.binary.bw.Write(param.fData[2]);
                        parent.binary.bw.Write(param.fData[3]);
                        break;
                    case Effect.Type.tmatrix2x2:
                        parent.binary.bw.Write((byte)(ParameterType.COMPONENT_2x2 | ParameterType.TYPE_TRANSPOSE_MATRIX));
                        parent.binary.bw.Write(param.fData[0]);
                        parent.binary.bw.Write(param.fData[1]);
                        parent.binary.bw.Write(param.fData[2]);
                        parent.binary.bw.Write(param.fData[3]);
                        break;
                    case Effect.Type.matrix3x3:
                        parent.binary.bw.Write((byte)(ParameterType.COMPONENT_3x3 | ParameterType.TYPE_MATRIX));
                        parent.binary.bw.Write(param.fData[0]);
                        parent.binary.bw.Write(param.fData[1]);
                        parent.binary.bw.Write(param.fData[2]);
                        parent.binary.bw.Write(param.fData[3]);
                        parent.binary.bw.Write(param.fData[4]);
                        parent.binary.bw.Write(param.fData[5]);
                        parent.binary.bw.Write(param.fData[6]);
                        parent.binary.bw.Write(param.fData[7]);
                        parent.binary.bw.Write(param.fData[8]);
                        break;
                    case Effect.Type.tmatrix3x3:
                        parent.binary.bw.Write((byte)(ParameterType.COMPONENT_3x3 | ParameterType.TYPE_TRANSPOSE_MATRIX));
                        parent.binary.bw.Write(param.fData[0]);
                        parent.binary.bw.Write(param.fData[1]);
                        parent.binary.bw.Write(param.fData[2]);
                        parent.binary.bw.Write(param.fData[3]);
                        parent.binary.bw.Write(param.fData[4]);
                        parent.binary.bw.Write(param.fData[5]);
                        parent.binary.bw.Write(param.fData[6]);
                        parent.binary.bw.Write(param.fData[7]);
                        parent.binary.bw.Write(param.fData[8]);
                        break;
                    case Effect.Type.matrix4x4:
                        parent.binary.bw.Write((byte)(ParameterType.COMPONENT_4x4 | ParameterType.TYPE_MATRIX));
                        parent.binary.bw.Write(param.fData[0]);
                        parent.binary.bw.Write(param.fData[1]);
                        parent.binary.bw.Write(param.fData[2]);
                        parent.binary.bw.Write(param.fData[3]);
                        parent.binary.bw.Write(param.fData[4]);
                        parent.binary.bw.Write(param.fData[5]);
                        parent.binary.bw.Write(param.fData[6]);
                        parent.binary.bw.Write(param.fData[7]);
                        parent.binary.bw.Write(param.fData[8]);
                        parent.binary.bw.Write(param.fData[9]);
                        parent.binary.bw.Write(param.fData[10]);
                        parent.binary.bw.Write(param.fData[11]);
                        parent.binary.bw.Write(param.fData[12]);
                        parent.binary.bw.Write(param.fData[13]);
                        parent.binary.bw.Write(param.fData[14]);
                        parent.binary.bw.Write(param.fData[15]);
                        break;
                    case Effect.Type.tmatrix4x4:
                        parent.binary.bw.Write((byte)(ParameterType.COMPONENT_4x4 | ParameterType.TYPE_TRANSPOSE_MATRIX));
                        parent.binary.bw.Write(param.fData[0]);
                        parent.binary.bw.Write(param.fData[1]);
                        parent.binary.bw.Write(param.fData[2]);
                        parent.binary.bw.Write(param.fData[3]);
                        parent.binary.bw.Write(param.fData[4]);
                        parent.binary.bw.Write(param.fData[5]);
                        parent.binary.bw.Write(param.fData[6]);
                        parent.binary.bw.Write(param.fData[7]);
                        parent.binary.bw.Write(param.fData[8]);
                        parent.binary.bw.Write(param.fData[9]);
                        parent.binary.bw.Write(param.fData[10]);
                        parent.binary.bw.Write(param.fData[11]);
                        parent.binary.bw.Write(param.fData[12]);
                        parent.binary.bw.Write(param.fData[13]);
                        parent.binary.bw.Write(param.fData[14]);
                        parent.binary.bw.Write(param.fData[15]);
                        break;
                    case Effect.Type.texture:
                        parent.binary.bw.Write((byte)(ParameterType.COMPONENT_1 | ParameterType.TYPE_TEXTURE));
                        parent.binary.WriteReference(param.sData);
                        break;
                    default:
                        parent.binary.bw.Write((byte)0);
                        break;
                }
            }
            parent.binary.CloseFile();
        }
    }
}
