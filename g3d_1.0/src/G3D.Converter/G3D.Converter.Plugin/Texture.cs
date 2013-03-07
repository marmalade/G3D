using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using G3D.Converter.Converter;
using G3D.Converter.Image;

namespace G3D.Converter.Plugin {
    public class TextureSettings : IConvertData {
        public string type { get { return "textureSettings"; } }

        public List<ImageFormat> formatChoices=new List<ImageFormat>();
    }
    public class Texture : IConvertData {
        public string type { get { return "texture"; } }
        public enum Filter {
            MIN_NONE = 0,
            MIN_NEAREST = 1,
            MIN_LINEAR = 2,
            MAG_NONE = 0,
            MAG_NEAREST = 4,
            MAG_LINEAR = 8,
            MIP_NONE = 0,
            MIP_NEAREST = 16,
            MIP_LINEAR = 32,
        }
        public enum Wrap {
            CLAMP,
            MIRRORED,
            REPEAT,
            BORDER,
            MIRROR_ONCE,
        }
        public class Image {
            public string filename;
            public string type="2d";  //((+|-)(x|y|z))|2d
            public ImageFormat format = new ImageFormat();
            public int level=0;
            public ImageData data=null;

            public Image Clone(ImageFormat format) {
                Image image=new Image();
                image.filename=filename;
                image.type=type;
                image.format=format;
                image.level=level;
                return image;
            }
        }
        public Filter filter=Filter.MIN_LINEAR|Filter.MAG_LINEAR|Filter.MIP_NONE;
        public Wrap s=Wrap.REPEAT;
        public Wrap t=Wrap.REPEAT;
        public int numMips=0;
        public List<Image> images=new List<Image>();
        public List<ImageFormat> formatChoices=new List<ImageFormat>();
    }
    public class ProcessTexture : ConvertItem<ConverterMgr,Texture> {
        Dictionary<int,Dictionary<string,List<Texture.Image>>> sorted;

        public ProcessTexture() : base("process.texture") { }
        protected override void Process(Texture texture) {
            AddFormats(texture);
            Sort(texture);
            Validate(texture);
            LoadData(texture);
            sorted.Clear();
        }

        private void AddFormats(Texture texture) {
            List<ImageFormat> formatChoices=texture.formatChoices;
            if(formatChoices.Count==0) {
                TextureSettings settings=mgr.GetData<TextureSettings>("texture.settings",GetterType.Fetch);
                if(settings!=null)
                    formatChoices=settings.formatChoices;
            }
            if(formatChoices.Count==0)
                return;

            foreach(Texture.Image image in texture.images.ToArray()) {
                if(image.format.depth!=ImageFormat.Depth.Unknown)
                    continue;

                image.format=formatChoices[0];
                for(int i=1;i<formatChoices.Count;i++)
                    texture.images.Add(image.Clone(formatChoices[i]));
            }
        }
        private void Sort(Texture texture) {
            sorted=new Dictionary<int,Dictionary<string,List<Texture.Image>>>();
            foreach(Texture.Image image in texture.images) {
                int hash=image.format.GetHashCode();
                if(!sorted.ContainsKey(hash))
                    sorted[hash]=new Dictionary<string,List<Texture.Image>>();
                if(!sorted[hash].ContainsKey(image.type))
                    sorted[hash][image.type]=new List<Texture.Image>();
                sorted[hash][image.type].Add(image);
            }
        }
        private void Validate(Texture texture) {
            List<string> map=(new string[] { "1d","2d","3d","+x","-x","+y","-y","+z","+z" }).ToList();
            foreach(var set in sorted.Values) {
                uint mask=0;
                foreach(var list in set) {
                    int index=map.IndexOf(list.Key);
                    if(index==-1)
                        mgr.Error(string.Format("image {0} has bad type {1}",mgr.name,list.Key));
                    if((mask&1<<index)==1<<index)
                        mgr.Error(string.Format("image {0} has 2 images of the same format and type",mgr.name));
                    mask|=(uint)(1<<index);

                    uint got=0;
                    foreach(Texture.Image image in list.Value) {
                        if((got&1<<image.level)==1<<image.level)
                            mgr.Error(string.Format("image {0} has 2 images of the same format type and level",mgr.name));
                        got|=(uint)(1<<image.level);
                    }
                    if((got&1)==0)
                        mgr.Error(string.Format("image {0} has no top level image for format and type",mgr.name));
                }
                if(mask!=1 && mask!=2 && mask!=4 && mask!=(8+16+32+64+128+256))
                    mgr.Error(string.Format("image {0} has a bad set of types",mgr.name));
            }
            mgr.Message("Validated texture");
        }
        private void LoadData(Texture texture) {
            foreach(var set in sorted.Values) {
                foreach(var list in set) {
                    foreach(Texture.Image image in list.Value) {
                        if(!File.Exists(image.filename)) {
                            mgr.Error("Image file: "+image.filename+" does not exist");
                            continue;
                        }
                        List<ImageData> result=new List<ImageData>();
                        mgr.Message("Reading texture: "+image.filename);
                        string err="";
                        if(image.level==0)
                            err=mgr.imageMgr.Load(image.filename,image.format,result,texture.numMips);
                        else
                            err=mgr.imageMgr.Load(image.filename,image.format,result,0);
                        if(err.Length>0)
                            mgr.Error("Image error: "+err);

                        foreach(ImageData idata in result) {
                            Texture.Image image2=list.Value.FirstOrDefault((i) => i.type==image.type && i.level==idata.m_MipLevel);
                            if(image2==image) {
                                image.data=idata;
                                image.format=idata.format;
                            }  else if(image2==null) {
                                image2=new Texture.Image();
                                image2.filename=image.filename;
                                image2.level=idata.m_MipLevel;
                                image2.type=image.type;
                                image2.data=idata;
                                image2.format=idata.format;
                                texture.images.Add(image2);
                            }
                        }
                    }
                }
            }
        }
    }
    public class ConfigureTexture : ConfigureSection {
        public ConfigureTexture() : base("configure.texture") { }
        protected override void Process(XElement item) {
            string name=(string)item.Attribute("name");
            if(name==null)
                name=mgr.name;
            else
                name=mgr.name+"."+name;
            Texture text=mgr.GetData<Texture>(name,GetterType.Create);
            if(text==null) return;

            string min=(string)item.Attribute("min");
            string mag=(string)item.Attribute("mag");
            string mip=(string)item.Attribute("mip");
            string s=(string)item.Attribute("s");
            string t=(string)item.Attribute("t");

            if(min!=null || mag!=null || mip!=null) {
                text.filter = 0;
                switch(min) {
                    case "nearest": text.filter |= Texture.Filter.MIN_NEAREST; break;
                    case "none": text.filter |= Texture.Filter.MIN_NONE; break;
                    default: text.filter |= Texture.Filter.MIN_LINEAR; break;
                }
                switch(mag) {
                    case "nearest": text.filter |= Texture.Filter.MAG_NEAREST; break;
                    case "none": text.filter |= Texture.Filter.MAG_NONE; break;
                    default: text.filter |= Texture.Filter.MAG_LINEAR; break;
                }
                switch(mip) {
                    case "linear": text.filter |= Texture.Filter.MIP_LINEAR; break;
                    case "none": text.filter |= Texture.Filter.MIP_NONE; break;
                    default: text.filter |= Texture.Filter.MIP_NEAREST; break;
                }
            }
            switch(s) {
                case "clamp": text.s = Texture.Wrap.CLAMP; break;
                case "mirrored": text.s = Texture.Wrap.MIRRORED; break;
                case "mirroronce": text.s = Texture.Wrap.MIRROR_ONCE; break;
                case "border": text.s = Texture.Wrap.BORDER; break;
                case "repeat": text.s = Texture.Wrap.REPEAT; break;
            }
            switch(t) {
                case "clamp": text.t = Texture.Wrap.CLAMP; break;
                case "mirrored": text.t = Texture.Wrap.MIRRORED; break;
                case "mirroronce": text.t = Texture.Wrap.MIRROR_ONCE; break;
                case "border": text.t = Texture.Wrap.BORDER; break;
                case "repeat": text.t = Texture.Wrap.REPEAT; break;
            }
            if(item.Attribute("nummips")!=null)
                text.numMips = (int)item.Attribute("nummips");

            if(text.images.Count>0)
                if(item.Attribute("type")!=null)
                    text.images[0].type = (string)item.Attribute("type");

            foreach(XElement child in item.Elements("format"))
                text.formatChoices.Add(LoadFormat(child));

            foreach(XElement xImage in item.Elements("image")) {
                Texture.Image image=new Texture.Image();
                image.filename=mgr.MakeAbsolute((string)xImage.Attribute("filename"),parent.filename);
                text.images.Add(image);

                if(xImage.Attribute("type")!=null)
                    image.type = (string)item.Attribute("type");
                if(xImage.Attribute("level")!=null)
                    image.level = (int)item.Attribute("level");
                image.format=LoadFormat(xImage.Element("format"));
            }
        }
        public static ImageFormat LoadFormat(XElement xFormat) {
            ImageFormat format=new ImageFormat();
            if(xFormat==null) return format;

            string depth=(string)xFormat.Attribute("depth");
            string type=(string)xFormat.Attribute("type");
            string layout=(string)xFormat.Attribute("layout");
            string compression=(string)xFormat.Attribute("compression");
            string flag=(string)xFormat.Attribute("flag");

            if(compression!=null) {
                format.depth=ImageFormat.Depth.Compressed;
                format.layout=(ImageFormat.Layout)Enum.Parse(typeof(ImageFormat.Layout),compression,true);
            } else {
                if(layout!=null)
                    format.layout=(ImageFormat.Layout)Enum.Parse(typeof(ImageFormat.Layout),layout,true);
                if(depth!=null) {
                    depth=depth.Replace(' ','_');
                    if(char.IsDigit(depth[0]))
                        depth="D"+depth;
                    format.depth=(ImageFormat.Depth)Enum.Parse(typeof(ImageFormat.Depth),depth,true);
                }
            }
            if(type!=null)
                format.type=(ImageFormat.Type)Enum.Parse(typeof(ImageFormat.Type),type.Replace(' ','_'),true);
            if(flag!=null)
                format.flag=(ImageFormat.Flag)Enum.Parse(typeof(ImageFormat.Flag),flag.Replace(' ','_'),true);

            xFormat=xFormat.Element("format_alpha");
            if(xFormat!=null)
                format.format_alpha=LoadFormat(xFormat);
            return format;
        }
    }
    public class TextureFile : ConvertItem<ConverterMgr,string> {
        public TextureFile() : base("texture") { }
        protected override void Process(string filename) {
            string name = Path.GetFileNameWithoutExtension(filename);
            Texture text=mgr.GetData<Texture>(name,GetterType.Create);
            if(text==null) return;

            Texture.Image image=new Texture.Image();
            image.filename=filename;
            text.images.Add(image);
        }
    }
    public class TextureTextWriter : ConvertItem<TextModule,Texture> {
        public TextureTextWriter() : base("write.texture") { }
        protected override void Process(Texture item) {
            bool cube=item.images.Any((i) => i.data!=null && (i.type[0]=='+' || i.type[0]=='-'));

            string filter=((Texture.Filter)(((int)item.filter)&3)).ToString().Substring(4);
            filter+="|"+((Texture.Filter)(((int)item.filter)&12)).ToString().Substring(4);
            filter+="|"+((Texture.Filter)(((int)item.filter)&48)).ToString().Substring(4);

            parent.lines.Add(string.Format("texture {0} {1} {2} {3} {4}",cube?"cube":"2d",filter,item.s,item.t,item.numMips));

            foreach(Texture.Image image in item.images) {
                if(image.data==null) continue;

                parent.lines.Add(string.Format("image {0} {1} {2} {3} {4}",image.type,image.level,image.data.m_Width,image.data.m_Height,image.data.m_BytesPerPixel));
                parent.lines.Add(string.Format("format {0} {1} {2} {3}",image.format.depth,image.format.type,image.format.layout,image.format.flag));
                parent.lines.Add(string.Format("idata {0}",image.data.m_Data.Length));

                int s1=image.data.m_BytesPerPixel;
                int s2=s1*16;
                for(int i=0;i<image.data.m_Data.Length;i+=s2) {
                    string line="i ";
                    for(int j=0;j<s2 && (i+j)<image.data.m_Data.Length;j+=s1) {
                        for(int k=0;k<s1 && (i+j+k)<image.data.m_Data.Length;k++)
                            line+=image.data.m_Data[i+j+k].ToString("x2");
                        line+=" ";
                    }
                    parent.lines.Add(line);
                }
            }
        }
    }
    public class TextureBinaryWriter : ConvertItem<BinaryModule,Texture> {
        public TextureBinaryWriter() : base("write.texture") { }
        protected override void Process(Texture item) {
            int num=item.images.Count((i) => i.data!=null);
            bool cube=item.images.Any((i) => i.data!=null && (i.type[0]=='+' || i.type[0]=='-'));

            parent.binary.OpenFile("TEXT",mgr.GetName(item));
            if(cube)
                parent.binary.bw.Write((byte)item.filter|128);
            else
                parent.binary.bw.Write((byte)item.filter);
            parent.binary.bw.Write((byte)item.s);
            parent.binary.bw.Write((byte)item.t);
            parent.binary.bw.Write((byte)item.numMips);
            parent.binary.bw.Write((uint)num);

            foreach(Texture.Image image in item.images) {
                if(image.data==null) continue;

                parent.binary.OpenFile("IMAG");

                parent.binary.bw.Write((byte)image.format.depth);
                parent.binary.bw.Write((byte)image.format.type);
                parent.binary.bw.Write((byte)image.format.layout);
                parent.binary.bw.Write((byte)image.format.flag);
                parent.binary.bw.Write((byte)image.type[0]);
                parent.binary.bw.Write((byte)image.type[1]);
                parent.binary.bw.Write((byte)image.level);
                parent.binary.bw.Write((byte)image.data.m_BytesPerPixel);
                parent.binary.bw.Write((ushort)image.data.m_Width);
                parent.binary.bw.Write((ushort)image.data.m_Height);
                parent.binary.bw.Write((uint)image.data.m_Data.Length);
                parent.binary.bw.Write(image.data.m_Data);

                parent.binary.CloseFile();
            }
            parent.binary.CloseFile();
        }
    }
    class ColladaTexture : ConvertItem<Collada,string> {
        public ColladaTexture() : base("collada.texture") { }
        protected override void Process(string tname) {
            string name=mgr.name+"."+tname;
            XElement xImage=parent.GetFromLib(null,tname,"image");
            if(xImage==null) {
                mgr.Error("cannot find image "+tname);
                return;
            }
            xImage=xImage.Element(parent.ns+"init_from");
            if(xImage==null) {
                mgr.Error("cannot find image init_from "+tname);
                return;
            }

            Texture text=mgr.GetData<Texture>(name,GetterType.Create);
            if(text==null) return;

            Texture.Image image=new Texture.Image();
            if(xImage.Value.StartsWith("file:///"))
                image.filename=xImage.Value.Substring(8);
            else
                image.filename=xImage.Value;
            image.filename=image.filename.Replace("%20"," ");
            text.images.Add(image);
        }
    }
}
