using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using G3D.Converter.Converter;

namespace G3D.Converter.Plugin {
    public class ModelSettings : IConvertData {
        public class Channel {
            public string name;
            public string semantic=null;
            public string set="";
            public int def=0;
            public bool useDefault=false;
        }

        public string type { get { return "modelSettings"; } }

        public bool optimise=true;
        public Dictionary<string,float> toleranceMap=new Dictionary<string,float>();
        public List<Channel> channels=new List<Channel>();
        public Model.IndexType outputTypes=0;

        public void MakeDefault() {
            toleranceMap["POSITION"]=0;
            toleranceMap["NORMAL"]=0.001f;
            toleranceMap["TEXCOORD"]=0.001f;
        }
        public void Clone(ModelSettings to) {
            to.optimise=optimise;
            to.toleranceMap=new Dictionary<string,float>(toleranceMap);
            to.channels=new List<Channel>(channels);
            to.outputTypes=outputTypes;
        }
    }
    public class Model : IConvertData {
        public enum IndexType {
            Lines=1,
            LineStrips=2,
            Triangles=4,
            TriStrip=8,
            TriFans=16,
            Polygons=32,
            QuadsAndTris=64,
        };
        public class SourceData {
            public string name;
            public float[] data;
        }
        public class SourceDataRef {
            public SourceData data;
            public int offset;
            public int stride;
        }
        public class SourceAccessor {
            public string name;
            public string semantic="";
            public int count;
            public List<SourceDataRef> dataRefs=new List<SourceDataRef>();

            public List<int> map=null;
            public List<float> data=null;
        }
        public class SourceAccessorRef {
            public string name;
            public string semantic;
            public SourceAccessor accessor;
        }
        public class SourceInput {
            public string name;
            public SourceAccessor data;
            public string semantic;
            public string set="";
            public int offset=-1;
        }
        public class SubModel {
            public IndexType type;
            public int indexStride;
            public int vertexStride;
            public List<SourceInput> inputList=new List<SourceInput>();
            public List<int> VertexMap=new List<int>();
            public List<int> countList=new List<int>();
            public List<int> IndexList=new List<int>();
            public List<float> VertexList=new List<float>();
            public int material;
        }

        public string type { get { return "model"; } }

        public List<SourceData> dataList=new List<SourceData>();
        public List<SourceAccessor> accessorList=new List<SourceAccessor>();
        public List<SubModel> subModelList=new List<SubModel>();
        public List<SourceAccessorRef> mapAccessorToInput=new List<SourceAccessorRef>();
        public List<string> materials=new List<string>();

        public ModelSettings settings=null;
    }
    class ColladaModel : ColladaSection {
        public ColladaModel() : base("collada.model") { }
        protected override void Process(XElement item) {
            string name=mgr.name+"."+(string)item.Attribute("id");
            XElement xMesh=item.Element(parent.ns+"mesh");
            if(xMesh==null) {
                mgr.Warning(parent.GetLocation(item),"Skipping geometry "+name+" as it does not have mesh data");
                return;
            }
            if(parent.nodeName=="")
                parent.nodeName=name;

            Model model=mgr.GetData<Model>(name,GetterType.CreateOnly);
            if(model==null) return;

            mgr.Message("Reading Model: "+name);

            Read(xMesh,model);
        }
        private void Read(XElement xMesh,Model model) {
            foreach(XElement xSource in xMesh.Elements(parent.ns+"source"))
                ReadData(model,xSource);
            foreach(XElement xSource in xMesh.Elements(parent.ns+"source"))
                ReadAccessor(model,xSource);

            ReadVertexData(model,xMesh);
            
            foreach(XElement xSub in xMesh.Elements()) {
                Model.SubModel sub;
                switch(xSub.Name.LocalName) {
                    case "lines": //1 p p:count*2*<num>
                        sub=MakeSubModel(xSub,Model.IndexType.Lines,model.materials);
                        sub.indexStride=ReadInputs(model,sub,xSub);
                        ReadIndexes(sub,xSub,2);
                        model.subModelList.Add(sub);
                        break;
                    case "linestrips": //count p p:<x>*<num>
                        sub=MakeSubModel(xSub,Model.IndexType.LineStrips,model.materials);
                        sub.indexStride=ReadInputs(model,sub,xSub);
                        ReadIndexes(sub,xSub,0);
                        model.subModelList.Add(sub);
                        break;
                    case "polylist": //1 vcount,1 p vcount:count, p:<vcount sum>*<num>*<count>
                        sub=MakeSubModel(xSub,Model.IndexType.Polygons,model.materials);
                        sub.indexStride=ReadInputs(model,sub,xSub);
                        ReadIndexes(sub,xSub,-1);
                        model.subModelList.Add(sub);
                        break;
                    case "triangles": //1 p p:count*3*<num>
                        sub=MakeSubModel(xSub,Model.IndexType.Triangles,model.materials);
                        sub.indexStride=ReadInputs(model,sub,xSub);
                        ReadIndexes(sub,xSub,3);
                        model.subModelList.Add(sub);
                        break;
                    case "trifans": //count p p:<x>*<num>
                        sub=MakeSubModel(xSub,Model.IndexType.TriFans,model.materials);
                        sub.indexStride=ReadInputs(model,sub,xSub);
                        ReadIndexes(sub,xSub,0);
                        model.subModelList.Add(sub);
                        break;
                    case "tristrips": //count p p:<x>*<num>
                        sub=MakeSubModel(xSub,Model.IndexType.TriStrip,model.materials);
                        sub.indexStride=ReadInputs(model,sub,xSub);
                        ReadIndexes(sub,xSub,0);
                        model.subModelList.Add(sub);
                        break;
                    case "polygons":
                        mgr.Error(parent.GetLocation(xSub),"polygons type not supported");
                        continue;
                    default:
                        continue;
                }
            }
        }
        private void ReadData(Model model,XElement xSource) {
            XElement xArray=xSource.Element(parent.ns+"float_array");
            if(xArray==null) return;

            Model.SourceData source=new Model.SourceData();
            source.name=(string)xArray.Attribute("id");

            source.data=xArray.Value.Split(" \t\n".ToArray(),StringSplitOptions.RemoveEmptyEntries).Select((s) => float.Parse(s)).ToArray();

            model.dataList.Add(source);
        }
        private void ReadAccessor(Model model,XElement xSource) {
            XElement accessor=xSource.Element(parent.ns+"technique_common");
            if(accessor==null) return;
            accessor=accessor.Element(parent.ns+"accessor");
            if(accessor==null) return;

            string dataName=((string)accessor.Attribute("source")).Substring(1);
            Model.SourceData dataItem=model.dataList.FirstOrDefault((d) => d.name==dataName);
            if(dataItem==null)
                return;

            Model.SourceAccessor source=new Model.SourceAccessor();
            source.name=(string)xSource.Attribute("id");
            source.count=(int)accessor.Attribute("count");

            int offset=(accessor.Attribute("offset")==null)?0:(int)accessor.Attribute("offset");
            int stride=(accessor.Attribute("stride")==null)?1:(int)accessor.Attribute("stride");

            foreach(XElement param in accessor.Elements(parent.ns+"param")) {
                if(param.Attribute("name")!=null) {
                    Model.SourceDataRef sourceRef=new Model.SourceDataRef();
                    sourceRef.data=dataItem;
                    sourceRef.offset=offset;
                    sourceRef.stride=stride;

                    source.dataRefs.Add(sourceRef);
                }
                offset++;
            }

            model.accessorList.Add(source);
        }
        private void ReadVertexData(Model model,XElement xMesh) {
            XElement xVertex=xMesh.Element(parent.ns+"vertices");
            if(xVertex==null) return;

            foreach(XElement xInput in xVertex.Elements(parent.ns+"input")) {
                string name=((string)xInput.Attribute("source")).Substring(1);

                Model.SourceAccessorRef item=new Model.SourceAccessorRef();
                item.name=(string)xVertex.Attribute("id");
                item.semantic=(string)xInput.Attribute("semantic");
                item.accessor=model.accessorList.FirstOrDefault((a) => a.name==name);
                model.mapAccessorToInput.Add(item);
            }
        }
        private Model.SubModel MakeSubModel(XElement xSub,Model.IndexType type,List<string> materials) {
            Model.SubModel sub=new Model.SubModel();

            string mat=(string)xSub.Attribute("material");
            sub.material=materials.IndexOf(mat);
            if(sub.material==-1) {
                sub.material=materials.Count;
                materials.Add(mat);
            }

            sub.type=type;
            sub.indexStride=0;
            return sub;
        }
        private int ReadInputs(Model model,Model.SubModel sub,XElement xSub) {
            int numOffsets=0;
            List<Model.SourceInput> list=new List<Model.SourceInput>();

            foreach(XElement xInput in xSub.Elements(parent.ns+"input")) {
                int offset=(int)xInput.Attribute("offset");
                if(offset+1>numOffsets)
                    numOffsets=offset+1;

                Model.SourceInput source=new Model.SourceInput();
                source.name=((string)xInput.Attribute("source")).Substring(1);
                source.data=model.accessorList.FirstOrDefault((s) => s.name==source.name);
                source.semantic=(string)xInput.Attribute("semantic");
                if(xInput.Attribute("set")!=null)
                    source.set=(string)xInput.Attribute("set");
                source.offset=offset;
                list.Add(source);
            }
            foreach(Model.SourceInput source in list) {
                if(source.data!=null) {
                    source.name=source.semantic+source.set;
                    source.data.semantic=source.name;
                    sub.inputList.Add(source);
                }
                foreach(Model.SourceAccessorRef item in model.mapAccessorToInput.Where((k) => k.name==source.name)) {
                    Model.SourceInput source2=new Model.SourceInput();
                    source2.name=item.semantic;
                    source2.data=item.accessor;
                    source2.semantic=item.semantic;
                    source2.set="";
                    source2.offset=source.offset;
                    while(char.IsDigit(source2.semantic.Last())) {
                        source2.set=source2.semantic.Last().ToString()+source2.set;
                        source2.semantic=source2.semantic.Substring(0,source2.semantic.Length-1);
                    }
                    if(source2.data!=null)
                        source2.data.semantic=source2.name;
                    sub.inputList.Add(source2);
                }
            }
            return numOffsets;
        }
        private void ReadIndexes(Model.SubModel sub,XElement xSub,int num) {
            int count=(int)xSub.Attribute("count");

            if(num>0)
                sub.countList.AddRange(Enumerable.Repeat(num,count));
            else if(num==-1) {
                XElement xVCount=xSub.Element(parent.ns+"vcount");
                if(xVCount==null) return;

                string[] sCounts=xVCount.Value.Split(" \t\n".ToArray(),StringSplitOptions.RemoveEmptyEntries);
                sub.countList.AddRange(sCounts.Select((s) => int.Parse(s)));
            }
            foreach(XElement xP in xSub.Elements(parent.ns+"p")) {
                string[] array=xP.Value.Split(" \t\n".ToArray(),StringSplitOptions.RemoveEmptyEntries);
                sub.VertexMap.AddRange(array.Select((s) => int.Parse(s)));

                if(num==0)
                    sub.countList.Add(array.Length/sub.indexStride);
            }
            for(int i=0;i<sub.VertexMap.Count/sub.indexStride;i++)
                sub.IndexList.Add(i);
        }
    }
    public class ConfigureModel : ConfigureSection {
        public ConfigureModel() : base("configure.model") { }
        protected override void Process(XElement item) {
            string name=(string)item.Attribute("name");
            if(name==null)
                name=mgr.name;
            else
                name=mgr.name+"."+name;
            Model model=mgr.GetData<Model>(name,GetterType.Create);
            if(model==null) return;

            ModelSettings settings=mgr.GetData<ModelSettings>("model.settings");

            model.settings=new ModelSettings();
            settings.Clone(model.settings);
            ProcessSettings(model.settings,item);
        }
        public static void ProcessSettings(ModelSettings model,XElement item) {
            if(item.Attribute("optimise")!=null)
                model.optimise=(bool)item.Attribute("optimise");

            foreach(XElement tol in item.Elements("tolerance"))
                model.toleranceMap[(string)tol.Attribute("name")]=(float)tol.Attribute("value");

            if(item.Attribute("outputtypes")!=null)
                foreach(Model.IndexType type in ((string)item.Attribute("outputtypes")).Split(',').Select((s) => Enum.Parse(typeof(Model.IndexType),s)))
                    model.outputTypes|=type;

            foreach(XElement channel in item.Elements("channel"))
                model.channels.Add(ProcessChannel(channel));
        }
        private static ModelSettings.Channel ProcessChannel(XElement xChannel) {
            ModelSettings.Channel channel=new ModelSettings.Channel();

            channel.name=(string)xChannel.Attribute("name");
            if(xChannel.Attribute("set")!=null)
                channel.set=(string)xChannel.Attribute("set");

            if(xChannel.Attribute("semantic")!=null)
                channel.semantic=(string)xChannel.Attribute("semantic");
            else {
                channel.semantic=channel.name;
                while(char.IsDigit(channel.semantic.Last())) {
                    channel.set=channel.semantic.Last().ToString()+channel.set;
                    channel.semantic=channel.semantic.Substring(0,channel.semantic.Length-1);
                }
            }

            if(xChannel.Attribute("default")!=null) {
                channel.def=(int)xChannel.Attribute("default");
                channel.useDefault=true;
            }
            return channel;
        }
    }
    public class ProcessModel : ConvertItem<ConverterMgr,Model> {
        public class Converter {
            public Model.IndexType source;
            public Model.IndexType dest;
            public Action<Model.SubModel> converter;

            public Converter(Model.IndexType _source,Model.IndexType _dest,Action<Model.SubModel> _converter) {
                source=_source;
                dest=_dest;
                converter=_converter;
            }
        }
        public List<Converter> converters=new List<Converter>();
        public ProcessModel() : base("process.model") {
            converters.Add(new Converter(Model.IndexType.Polygons,Model.IndexType.Triangles,PolyToTris));
            converters.Add(new Converter(Model.IndexType.Polygons,Model.IndexType.QuadsAndTris,PolyToQuadTris));
        }

        protected override void Process(Model item) {
            ModelSettings settings;
            if(item.settings!=null)
                settings=item.settings;
            else
                settings=mgr.GetData<ModelSettings>("model.settings");

            foreach(Model.SourceAccessor source in item.accessorList)
                Remap(source,source.semantic,settings);

            foreach(Model.SubModel sub in item.subModelList)
                Process(sub,settings);
        }
        private void Process(Model.SubModel sub,ModelSettings settings) {
            RemapInputs(sub,settings);
            RemapOffsets(sub);

            foreach(Model.SourceInput input in sub.inputList)
                if(input.data!=null)
                    ApplyMap(sub,input.data,input.offset);

            if(settings.optimise)
                Remap(sub);

            if((sub.type&settings.outputTypes)!=sub.type) {
                Converter conv=converters.FirstOrDefault((c) => c.source==sub.type && (c.dest&settings.outputTypes)==c.dest);
                if(conv!=null)
                    conv.converter(sub);
                else
                    mgr.Error(string.Format("Cannot convert {0} to a suitible format",sub.type));
            }

            BuildVertexData(sub,settings);
        }
        private void RemapInputs(Model.SubModel sub,ModelSettings settings) {
            if(settings.channels.Count<=0)
                return;

            List<Model.SourceInput> newInputs=new List<Model.SourceInput>();
            foreach(ModelSettings.Channel channel in settings.channels) {
                Model.SourceInput input=sub.inputList.FirstOrDefault((i) => i.semantic==channel.semantic && i.set==channel.set);
                if(input==null && !channel.useDefault)
                    continue;

                Model.SourceInput newInput=new Model.SourceInput();
                newInput.name=channel.name;
                newInput.data=(input!=null)?input.data:null;
                newInput.semantic=channel.semantic;
                newInput.set=channel.set;
                newInput.offset=(input!=null)?input.offset:channel.def;
                if(newInput.data!=null)
                    newInput.data.semantic=newInput.name;
                newInputs.Add(newInput);
            }
            sub.inputList=newInputs;
        }
        private void RemapOffsets(Model.SubModel sub) {
            List<int> newMap=new List<int>();

            for(int i=0;i<sub.VertexMap.Count;i+=sub.indexStride)
                foreach(Model.SourceInput input in sub.inputList)
                    if(input.data==null)
                        newMap.Add(input.offset);
                    else
                        newMap.Add(sub.VertexMap[i+input.offset]);
            sub.VertexMap=newMap;

            sub.indexStride=0;
            foreach(Model.SourceInput input in sub.inputList)
                input.offset=sub.indexStride++;
        }
        private void Remap(Model.SourceAccessor source,string semantic,ModelSettings settings) {
            float tolerance=0.01f;
            if(settings.toleranceMap.ContainsKey(semantic))
                tolerance=settings.toleranceMap[semantic];
            if(!settings.optimise) tolerance=0;

            source.data=new List<float>();
            source.map=new List<int>();

            int stride=source.dataRefs.Count;
            int num=0;
            for(int i=0;i<source.count;i++) {
                List<float> item=new List<float>();
                foreach(Model.SourceDataRef dataRef in source.dataRefs)
                    item.Add(dataRef.data.data[dataRef.stride*i + dataRef.offset]);

                if(tolerance!=0) {
                    bool found=false;
                    for(int j=0;j<num;j++) {
                        found=true;
                        for(int k=0;k<stride;k++)
                            if(Math.Abs(item[k]-source.data[j*stride+k])>tolerance) {
                                found=false;
                                break;
                            }
                        if(found) {
                            source.map.Add(j);
                            break;
                        }
                    }
                    if(found) continue;
                }

                source.data.AddRange(item);
                source.map.Add(num);
                num++;
            }
        }
        private void ApplyMap(Model.SubModel sub,Model.SourceAccessor sourceData,int offset) {
            if(sourceData.map==null)
                return;
            for(int i=0;i<sub.VertexMap.Count;i+=sub.indexStride)
                sub.VertexMap[i+offset]=sourceData.map[sub.VertexMap[i+offset]];
        }
        private void Remap(Model.SubModel sub) {
            List<int> newData=new List<int>();
            sub.IndexList.Clear();

            for(int i=0;i<sub.VertexMap.Count;i+=sub.indexStride) {
                bool found=false;
                for(int j=0;j<newData.Count;j+=sub.indexStride) {
                    found=true;
                    for(int k=0;k<sub.indexStride;k++)
                        if(sub.VertexMap[i+k]!=newData[j+k]) {
                            found=false;
                            break;
                        }
                    if(found) {
                        sub.IndexList.Add(j/sub.indexStride);
                        break;
                    }
                }
                if(found)
                    continue;

                sub.IndexList.Add(newData.Count/sub.indexStride);
                for(int k=0;k<sub.indexStride;k++)
                    newData.Add(sub.VertexMap[i+k]);
            }
            sub.VertexMap=newData;
        }
        void PolyToTris(Model.SubModel sub) {
            List<int> IndexList=sub.IndexList;
            sub.IndexList=new List<int>();
            int offset=0;
            for(int i=0;i<sub.countList.Count;i++) {
                int count=sub.countList[i];
                if(count==3) {
                    for(int j=0;j<count;j++)
                        sub.IndexList.Add(IndexList[offset+j]);
                    offset+=count;
                    continue;
                }
                sub.countList[i]=0;

                int a=offset,b=offset+count-1;
                bool left=true;
                while(b-a>1) {
                    if(left) {
                        sub.IndexList.Add(IndexList[b]);
                        sub.IndexList.Add(IndexList[a]);
                        a++;
                        sub.IndexList.Add(IndexList[a]);
                        left=false;
                    } else {
                        sub.IndexList.Add(IndexList[b]);
                        sub.IndexList.Add(IndexList[a]);
                        b--;
                        sub.IndexList.Add(IndexList[b]);
                        left=true;
                    }
                    sub.countList[i]+=3;
                }
                offset+=count;
            }
            sub.type=Model.IndexType.Triangles;
        }
        void PolyToQuadTris(Model.SubModel sub) {
            List<int> IndexList=sub.IndexList;
            sub.IndexList=new List<int>();
            int offset=0;
            for(int i=0;i<sub.countList.Count;i++) {
                int count=sub.countList[i];
                if(count==4 || count==3) {
                    for(int j=0;j<count;j++)
                        sub.IndexList.Add(IndexList[offset+j]);
                    offset+=count;
                    continue;
                }
                sub.countList[i]=0;

                int a=offset,b=offset+count-1;
                bool left=true;
                while(b-a>1) {
                    if(left) {
                        sub.IndexList.Add(IndexList[b]);
                        sub.IndexList.Add(IndexList[a]);
                        a++;
                        sub.IndexList.Add(IndexList[a]);
                        left=false;
                    } else {
                        sub.IndexList.Add(IndexList[b]);
                        sub.IndexList.Add(IndexList[a]);
                        b--;
                        sub.IndexList.Add(IndexList[b]);
                        left=true;
                    }
                    sub.countList[i]+=3;
                }
                offset+=count;
            }
            sub.type=Model.IndexType.QuadsAndTris;
        }
        private void BuildVertexData(Model.SubModel sub,ModelSettings settings) {
            sub.vertexStride=0;
            foreach(Model.SourceInput input in sub.inputList)
                if(input.data!=null)
                    sub.vertexStride+=input.data.dataRefs.Count;

            for(int i=0;i<sub.VertexMap.Count/sub.indexStride;i++)
                foreach(Model.SourceInput input in sub.inputList)
                    if(input.data!=null)
                        for(int j=0;j<input.data.dataRefs.Count;j++)
                            sub.VertexList.Add(input.data.data[sub.VertexMap[i*sub.indexStride+input.offset]*input.data.dataRefs.Count+j]);
        }
    }
    public class ModelTextWriter : ConvertItem<TextModule,Model> {
        public ModelTextWriter() : base("write.model") { }
        protected override void Process(Model item) {
            foreach(Model.SubModel sub in item.subModelList) {
                parent.lines.Add(string.Format("material {0}",sub.material));
                parent.lines.Add(string.Format("vb {0} {1}",sub.VertexList.Count/sub.vertexStride,sub.vertexStride*sizeof(float)));
                for(int i=0;i<sub.VertexList.Count/sub.vertexStride;i++)
                    parent.lines.Add("v "+string.Join(" ",sub.VertexList.Skip(i*sub.vertexStride).Take(sub.vertexStride).Select((f) => f.ToString()).ToArray()));
                parent.lines.Add("");

                int offset=0;
                foreach(Model.SourceInput input in sub.inputList) {
                    if(input.data==null)
                        continue;
                    parent.lines.Add(string.Format("ve {0} {1} 32 float {2}",input.semantic+input.set,input.data.dataRefs.Count,offset));
                    offset+=input.data.dataRefs.Count*sizeof(float);
                }
                parent.lines.Add("");

                parent.lines.Add(string.Format("ib {0} 2",sub.IndexList.Count));
                offset=0;
                for(int i=0;i<sub.countList.Count;i++) {
                    parent.lines.Add("i "+string.Join(" ",sub.IndexList.Skip(offset).Take(sub.countList[i]).Select((v) => v.ToString()).ToArray()));
                    offset+=sub.countList[i];
                }
                parent.lines.Add(string.Format("is {0} 16",sub.type.ToString()));
                parent.lines.Add("");
            }
        }
    }
    internal class ModelBinaryWriter : ConvertItem<BinaryModule,Model> {
        enum ElementType {
            COMPONENT_1 = 0,
            COMPONENT_2 = 1,
            COMPONENT_3 = 2,
            COMPONENT_4 = 3,
            COMPONENT_MASK = 3,

            SIZE_8 = 4,
            SIZE_16 = 8,
            SIZE_32 = 12,
            SIZE_MASK = 12,

            TYPE_FLOAT = 16,
            TYPE_UNSIGNED = 32,
            TYPE_SIGNED = 48,
            TYPE_MASK = 48,

            NORMALIZE = 64,
            SRGB = 128,
        };
        enum IndexType {
            POINTS = 0,
            LINES = 1,
            LINESTRIP = 2,
            TRIANGLES = 3,
            TRIANGLESTRIP = 4,
            LINELOOP = 5,
            TRIANGLEFAN = 6,
            MASK = 7,

            SIZE_8 = 8,
            SIZE_16 = 16,
            SIZE_32 = 24,
            SIZE_MASK = 24,
        };
        public ModelBinaryWriter() : base("write.model") { }
        protected override void Process(Model item) {
            parent.binary.OpenFile("MODL",mgr.GetName(item));

            int num=item.subModelList.Count;
            parent.binary.bw.Write((byte)(num*3));
            parent.binary.bw.Write((byte)(num*2));

            for(int i=0;i<num;i++) {
                parent.binary.bw.Write((byte)(item.subModelList[i].material | 128));
                parent.binary.bw.Write((byte)(i*2));
                parent.binary.bw.Write((byte)(i*2+1));
            }

            foreach(Model.SubModel sub in item.subModelList) {
                parent.binary.OpenFile("VERT");
                parent.binary.bw.Write((uint)(sub.VertexList.Count/sub.vertexStride));
                parent.binary.bw.Write((uint)(sub.vertexStride*sizeof(float)));

                int count=0;
                foreach(Model.SourceInput input in sub.inputList) {
                    if(input.data==null)
                        continue;
                    count++;
                }
                parent.binary.bw.Write((uint)count);

                foreach(float f in sub.VertexList)
                    parent.binary.bw.Write(f);

                int offset=0;
                foreach(Model.SourceInput input in sub.inputList) {
                    if(input.data==null)
                        continue;
                    ElementType type;
                    switch(input.data.dataRefs.Count) {
                        case 1: type = ElementType.COMPONENT_1 | ElementType.SIZE_32 | ElementType.TYPE_FLOAT; break;
                        case 2: type = ElementType.COMPONENT_2 | ElementType.SIZE_32 | ElementType.TYPE_FLOAT; break;
                        case 3: type = ElementType.COMPONENT_3 | ElementType.SIZE_32 | ElementType.TYPE_FLOAT; break;
                        default: type = ElementType.COMPONENT_4 | ElementType.SIZE_32 | ElementType.TYPE_FLOAT; break;
                    }
                    parent.binary.WriteString(input.semantic+input.set);
                    parent.binary.bw.Write((uint)type);
                    parent.binary.bw.Write((uint)offset);
                    offset+=input.data.dataRefs.Count*sizeof(float);
                }
                parent.binary.CloseFile();

                int stride=2;
                if(sub.IndexList.Count>65535) stride=4;

                parent.binary.OpenFile("INDX");
                parent.binary.bw.Write((uint)(sub.IndexList.Count));
                parent.binary.bw.Write((uint)(stride));

                IndexType indexType = IndexType.TRIANGLES;
                switch(sub.type) {
                    case Model.IndexType.Lines:
                        indexType=IndexType.LINES;
                        break;
                    case Model.IndexType.LineStrips:
                        indexType=IndexType.LINESTRIP;
                        break;
                    case Model.IndexType.Triangles:
                        indexType=IndexType.TRIANGLES;
                        break;
                    case Model.IndexType.TriStrip:
                        indexType=IndexType.TRIANGLESTRIP;
                        break;
                    case Model.IndexType.TriFans:
                        indexType=IndexType.TRIANGLEFAN;
                        break;
                    default:
                        mgr.Error("invalid index type "+sub.type.ToString());
                        break;
                }
                if(stride==2) {
                    parent.binary.bw.Write((byte)(indexType | IndexType.SIZE_16));
                    foreach(int i in sub.IndexList)
                        parent.binary.bw.Write((ushort)i);
                } else {
                    parent.binary.bw.Write((byte)(indexType | IndexType.SIZE_32));
                    foreach(int i in sub.IndexList)
                        parent.binary.bw.Write((uint)i);
                }
                parent.binary.CloseFile();
            }
            parent.binary.CloseFile();
        }
    }
}