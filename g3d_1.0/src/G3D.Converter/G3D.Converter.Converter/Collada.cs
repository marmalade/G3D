using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Xml.Linq;

namespace G3D.Converter.Converter {
    public class ConfigureCollada : ConfigureSection {
        public ConfigureCollada() : base("configure.collada") { }
        protected override void Process(XElement item) {
            foreach(XElement child in item.Elements())
                mgr.Process("configure."+child.Name.LocalName,child);
        }
    }
    public abstract class ColladaSection : ConvertItem<Collada,XElement> {
        public ColladaSection(string name) : base(name) { }
    }
    public class Collada : ConvertItem<ConverterMgr,string> {
        public enum Axis {
            x_up, y_up, z_up,
            x_down, y_down, z_down,
        }

        public string filename;
        Dictionary<string,string> mapping=new Dictionary<string,string>();
        XElement root;
        public string ns;
        public float unit=1;
        public Axis[] axisList=new Axis[] { Axis.x_up,Axis.y_up,Axis.z_up };
        public List<string> materials=new List<string>();
        public string nodeName;

        public Collada() : base("collada") {
            mapping["animation"]="library_animations";
            mapping["animation_clip"]="library_animation_clips";
            mapping["camera"]="library_cameras";
            mapping["controller"]="library_controllers";
            mapping["effect"]="library_effects";
            mapping["force_field"]="library_force_fields";
            mapping["geometry"]="library_geometries";
            mapping["image"]="library_images";
            mapping["light"]="library_lights";
            mapping["material"]="library_materials";
            mapping["node"]="library_nodes";
            mapping["physics_material"]="library_physics_materials";
            mapping["physics_model"]="library_physics_models";
            mapping["physics_scene"]="library_physics_scenes";
            mapping["visual_scene"]="library_visual_scenes";
        }

        protected override void Process(string _filename) {
            filename=_filename;
            mgr.Message("Converting file: "+name);

            XDocument doc=XDocument.Load(filename,LoadOptions.SetLineInfo);
            root=doc.Root;
            ns="{"+root.Name.NamespaceName+"}";

            string version=root.Attribute("version").Value.Replace(".","");
            if(root.Name.LocalName!="COLLADA" || int.Parse(version)<140) {
                mgr.Error("not a valid collada file: "+filename);
                return;
            }

            ReadAsset(root.Element(ns+"asset"));
            mgr.Process("collada.scene",root.Element(ns+"scene"));
        }
        private void ReadAsset(XElement asset) {
            if(asset==null) return;

            XElement xUnit=asset.Element(ns+"unit");
            if(xUnit!=null)
                unit=float.Parse(xUnit.Attribute("meter").Value);

            XElement xAxis=asset.Element(ns+"up_axis");
            if(xAxis==null) return;
            switch(xAxis.Value) {
                case "X_UP":
                    axisList=new Axis[] { Axis.y_down,Axis.x_up,Axis.z_up };
                    break;
                case "Z_UP":
                    axisList=new Axis[] { Axis.x_up,Axis.z_up,Axis.y_down };
                    break;
                default:
                    axisList=new Axis[] { Axis.x_up,Axis.y_up,Axis.z_up };
                    break;
            }
        }/*
        private void ReadNode(string prefix,XElement xNode) {
            string name=(string)xNode.Attribute("name");
            if(name.Contains('.')) {
                mgr.Warning(GetLocation(xNode),"Item "+name+" contains . in name (removing all .)");
                name=name.Replace(".","");
            }
            name=prefix+"."+name;
            mgr.Message("Reading Node: "+name);

            mgr.Process("dae.object",xNode);
            mgr.Process("dae.frame",xNode);

            XElement xInst=xNode.Element(ns+"instance_geometry");
            if(xInst!=null)
                mgr.ReadSection("dae.geominst",name,xInst);

            foreach(XElement xNode2 in xNode.Elements(ns+"node"))
                ReadNode(name,xNode2);
        }*/
        public string GetLocation(XElement xNode) {
            if(((IXmlLineInfo)xNode).HasLineInfo())
                return string.Format("{0}({1:000},{2:000})",filename,((IXmlLineInfo)xNode).LineNumber,((IXmlLineInfo)xNode).LinePosition);
            else
                return string.Format("{0}",filename);
        }
        public XElement GetFromLib(XElement element,string attr,string type) {
            if(!mapping.ContainsKey(type)) return null;
            if(element!=null) {
                attr=(string)element.Attribute(attr);
                if(attr.Length<2) return null;
                attr=attr.Substring(1);
            }

            IEnumerable<XElement> list=root.Element(ns+mapping[type]).Elements(ns+type);
            return list.FirstOrDefault((x) => (string)x.Attribute("id")==attr);
        }
    }
}


