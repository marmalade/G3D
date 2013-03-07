using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using G3D.Converter.Converter;
using Microsoft.Xna.Framework;

namespace G3D.Converter.Plugin {
    public class NodeSettings : IConvertData {
        public string type { get { return "nodeSettings"; } }
        public Dictionary<string,float> toleranceMap=new Dictionary<string,float>();
        public string defaultScene="simple";

        public void MakeDefault() {
            toleranceMap["VECTOR"]=0.00001f;
            toleranceMap["ANGLE"]=0.001f;
        }
        public float GetTolerance(string name) {
            float tolerance=0.01f;
            if(toleranceMap.ContainsKey(name))
                tolerance=toleranceMap[name];
            return tolerance;
        }
    }
    public class Node : IConvertData {
        public enum DataType {
            node,       //no data
            joint,      //no data, tagged as a joint
            camera,     //cameraData
            geometry,   //model and materials
            controller, //model and materials (todo skeleton)
            light,      //(todo light)
        }
        public enum FrameType {
            lookat,     //vec1:eye vec2:at vec3:up
            matrix,     //mat:matrix
            rotate,     //vec1:axis value:angle
            rotateX,    //vec1:1,0,0 value:angle
            rotateY,    //vec1:0,1,0 value:angle
            rotateZ,    //vec1:0,0,1 value:angle
            scale,      //vec1:scale
            skew,       //value:angle vec1:rot axis vec2:trans axis
            translate   //vec1:translate
        }
        public class FrameData {
            public FrameType type;
            public string name;
            public Vector3 vec1;
            public Vector3 vec2;
            public Vector3 vec3;
            public Matrix mat;
            public float value;

            public FrameData(FrameType _type,string _name) { type=_type; name=_name; }
        }
        public class CameraData {
            public bool ortho=false;
            public float x=0;
            public float y=0;
            public float aspect=0;
            public float znear=0;
            public float zfar=0;
        }
        public string type { get { return "node"; } }

        public List<string> children=new List<string>();

        //intermediate data
        public List<FrameData> frameData=new List<FrameData>();
        public Matrix finalMat=Matrix.Identity;

        public DataType dataType=DataType.node;
        public CameraData camera=null;
        public string model="";
        public List<string> materials=new List<string>();
    }
    class ColladaNode : ColladaSection {
        public ColladaNode() : base("collada.node") { }
        protected override void Process(XElement item) {
            NodeSettings settings=mgr.GetData<NodeSettings>("node.settings",GetterType.Fetch);

            string parentName=parent.nodeName;
            Node parentNode=mgr.GetData<Node>(parentName);
            parent.nodeName+="."+(string)item.Attribute("name");

            Node node=mgr.GetData<Node>(parent.nodeName,GetterType.CreateOnly);
            if(node==null) {
                //if already exists add to new parent
                node=mgr.GetData<Node>(parent.nodeName);
                if(node!=null && parentNode!=null)
                    parentNode.children.Add(parent.nodeName);

                parent.nodeName=parentName;
                return;
            }

            if(parentNode!=null)
                parentNode.children.Add(parent.nodeName);

            ReadFrame(node,item,settings);

            foreach(XElement child in item.Elements(parent.ns+"node"))
                mgr.Process("collada.node",child);

            foreach(XElement child in item.Elements(parent.ns+"instance_node")) {
                XElement xNode=parent.GetFromLib(child,"url","node");
                if(xNode==null) continue;

                mgr.Process("collada.node",xNode);
            }

            if((string)item.Attribute("type")=="JOINT")
                node.dataType=Node.DataType.joint;

            XElement geom=item.Element(parent.ns+"instance_geometry");
            XElement ctr=item.Element(parent.ns+"instance_controller");
            XElement cam=item.Element(parent.ns+"instance_camera");
            XElement light=item.Element(parent.ns+"instance_light");

            if(ctr!=null) {
                node.dataType=Node.DataType.controller;
                ReadMaterials(node,ctr);
                ReadCtrInst(node,ctr);
            } else if(geom!=null) {
                node.dataType=Node.DataType.geometry;
                XElement xGeom=parent.GetFromLib(geom,"url","geometry");
                if(xGeom!=null) {
                    node.model=mgr.name+"."+((string)geom.Attribute("url")).Substring(1);
                    mgr.Process("collada.model",xGeom);
                }
                ReadMaterials(node,geom);
            } else if(cam!=null) {
                node.dataType=Node.DataType.camera;
                ReadCamera(node,cam);
            } else if(light!=null) {
                node.dataType=Node.DataType.light;
                //TODO ReadLight(node,light);
            }
            parent.nodeName=parentName;
        }

        private void ReadMaterials(Node node,XElement geom) {
            XElement bind=geom.Element(parent.ns+"bind_material");
            if(bind==null) return;

            XElement common=bind.Element(parent.ns+"technique_common");
            if(common==null) return;

            foreach(XElement mat in common.Elements(parent.ns+"instance_material")) {
                XElement xMat=parent.GetFromLib(mat,"target","material");
                if(xMat==null) continue;

                node.materials.Add(mgr.name+"."+((string)mat.Attribute("target")).Substring(1));
                mgr.Process("collada.material",xMat);

                //TODO bind vertex input?
            }
        }
        private void ReadCtrInst(Node node,XElement ctr) {
            XElement xCtr=parent.GetFromLib(ctr,"url","controller");
            if(xCtr==null) return;

            XElement skin=xCtr.Element(parent.ns+"skin");
            if(skin==null) return;

            XElement xGeom=parent.GetFromLib(skin,"source","geometry");
            if(xGeom!=null) {
                node.model=mgr.name+((string)skin.Attribute("source")).Substring(1);
                mgr.Process("collada.model",xGeom);
            }

            //TODO read skeleton/skin
        }
        private void ReadCamera(Node node,XElement cam) {
            XElement xCam=parent.GetFromLib(cam,"url","camera");
            if(xCam==null) return;

            XElement optics=xCam.Element(parent.ns+"optics");
            if(optics==null) return;

            XElement common=optics.Element(parent.ns+"technique_common");
            if(common==null) return;

            XElement ortho=common.Element(parent.ns+"orthographic");
            XElement persp=common.Element(parent.ns+"perspective");

            node.camera=new Node.CameraData();
            XElement x=null,y=null,a=null,n=null,f=null;
            if(persp!=null) {
                node.camera.ortho=false;

                x=persp.Element(parent.ns+"xfov");
                y=persp.Element(parent.ns+"yfov");
                a=persp.Element(parent.ns+"aspect_ratio");
                n=persp.Element(parent.ns+"znear");
                f=persp.Element(parent.ns+"zfar");
            } else if(ortho!=null) {
                node.camera.ortho=true;
                x=ortho.Element(parent.ns+"xmag");
                y=ortho.Element(parent.ns+"ymag");
                a=ortho.Element(parent.ns+"aspect_ratio");
                n=ortho.Element(parent.ns+"znear");
                f=ortho.Element(parent.ns+"zfar");
            }

            if(x!=null) node.camera.x=float.Parse(x.Value);
            if(y!=null) node.camera.y=float.Parse(y.Value);
            if(a!=null) node.camera.aspect=float.Parse(a.Value);
            if(n!=null) node.camera.znear=float.Parse(n.Value);
            if(f!=null) node.camera.zfar=float.Parse(f.Value);
        }
        private void ReadFrame(Node node,XElement item,NodeSettings settings) {
            node.finalMat=Matrix.Identity;
            foreach(XElement child in item.Elements()) {
                switch(child.Name.LocalName) {
                    case "lookat":
                        ReadLookAt(child,node,settings);
                        break;
                    case "matrix":
                        ReadMatrix(child,node);
                        break;
                    case "rotate":
                        ReadRotate(child,node,settings);
                        break;
                    case "scale":
                        ReadScale(child,node,settings);
                        break;
                    case "skew":
                        ReadSkew(child,node,settings);
                        break;
                    case "translate":
                        ReadTranslate(child,node,settings);
                        break;
                }
            }
        }
        private void ReadLookAt(XElement xNode,Node node,NodeSettings settings) {
            Node.FrameData frame=new Node.FrameData(Node.FrameType.lookat,(string)xNode.Attribute("sid"));
            frame.vec1=ReadVector3(xNode,0);
            frame.vec2=ReadVector3(xNode,3);
            frame.vec3=ReadVector3(xNode,6);
            if(IsZero(frame.vec1,settings.GetTolerance("VECTOR")) && IsZero(frame.vec2,settings.GetTolerance("VECTOR")) && IsZero(frame.vec3,settings.GetTolerance("VECTOR")))
                return;
            node.finalMat*=Matrix.CreateLookAt(frame.vec1,frame.vec2,frame.vec3);
            node.frameData.Add(frame);
        }
        private void ReadMatrix(XElement xNode,Node node) {
            Node.FrameData frame=new Node.FrameData(Node.FrameType.matrix,(string)xNode.Attribute("sid"));
            frame.mat=ReadMatrix(xNode);
            node.finalMat*=frame.mat;
            node.frameData.Add(frame);
        }
        private void ReadRotate(XElement xNode,Node node,NodeSettings settings) {
            Node.FrameData frame=new Node.FrameData(Node.FrameType.rotate,(string)xNode.Attribute("sid"));
            frame.vec1=ReadVector3(xNode,0);
            frame.value=MathHelper.ToRadians(ReadFloat(xNode,3));
            if(IsZero(frame.value,settings.GetTolerance("ANGLE")))
                return;
            if(IsZero(frame.vec1-new Vector3(1,0,0),settings.GetTolerance("VECTOR")))
                frame.type=Node.FrameType.rotateX;
            else if(IsZero(frame.vec1-new Vector3(0,1,0),settings.GetTolerance("VECTOR")))
                frame.type=Node.FrameType.rotateY;
            else if(IsZero(frame.vec1-new Vector3(0,0,1),settings.GetTolerance("VECTOR")))
                frame.type=Node.FrameType.rotateZ;
            node.finalMat*=Matrix.CreateFromAxisAngle(frame.vec1,frame.value);
            node.frameData.Add(frame);
        }
        private void ReadScale(XElement xNode,Node node,NodeSettings settings) {
            Node.FrameData frame=new Node.FrameData(Node.FrameType.scale,(string)xNode.Attribute("sid"));
            frame.vec1=ReadVector3(xNode,0);
            if(IsZero(frame.vec1-new Vector3(1,1,1),settings.GetTolerance("VECTOR")))
                return;
            node.finalMat*=Matrix.CreateScale(frame.vec1);
            node.frameData.Add(frame);
        }
        private void ReadSkew(XElement xNode,Node node,NodeSettings settings) {
            Node.FrameData frame=new Node.FrameData(Node.FrameType.skew,(string)xNode.Attribute("sid"));
            frame.value=MathHelper.ToRadians(ReadFloat(xNode,0));
            frame.vec1=ReadVector3(xNode,1);
            frame.vec2=ReadVector3(xNode,4);
            if(IsZero(frame.value,settings.GetTolerance("ANGLE")))
                return;
            //TODO apply skew to matrix
            node.frameData.Add(frame);
        }
        private void ReadTranslate(XElement xNode,Node node,NodeSettings settings) {
            Node.FrameData frame=new Node.FrameData(Node.FrameType.translate,(string)xNode.Attribute("sid"));
            frame.vec1=ReadVector3(xNode,0);
            if(IsZero(frame.vec1,settings.GetTolerance("VECTOR")))
                return;
            node.finalMat*=Matrix.CreateTranslation(frame.vec1);
            node.frameData.Add(frame);
        }
        private bool IsZero(Vector3 vec,float tolerence) {
            return IsZero(vec.X,tolerence) && IsZero(vec.Y,tolerence) && IsZero(vec.Z,tolerence);
        }
        private bool IsZero(float value,float tolerence) {
            return Math.Abs(value)<tolerence;
        }
        private Matrix ReadMatrix(XElement xNode) {
            return new Matrix(ReadFloat(xNode,0),ReadFloat(xNode,1),ReadFloat(xNode,2),ReadFloat(xNode,3),
                ReadFloat(xNode,4),ReadFloat(xNode,5),ReadFloat(xNode,6),ReadFloat(xNode,7),
                ReadFloat(xNode,8),ReadFloat(xNode,9),ReadFloat(xNode,10),ReadFloat(xNode,11),
                ReadFloat(xNode,12),ReadFloat(xNode,13),ReadFloat(xNode,14),ReadFloat(xNode,15));
        }
        private Vector3 ReadVector3(XElement xNode,int offset) {
            return new Vector3(ReadFloat(xNode,offset),ReadFloat(xNode,offset+1),ReadFloat(xNode,offset+2));
        }
        private float ReadFloat(XElement xNode,int offset) {
            string[] values=xNode.Value.Split(" \t\n".ToArray(),StringSplitOptions.RemoveEmptyEntries).ToArray();

            float value=0;
            if(offset>=values.Length)
                mgr.Error(parent.GetLocation(xNode),"not enough values");
            else if(!float.TryParse(values[offset],out value))
                mgr.Error(parent.GetLocation(xNode),"bad value");

            return value;
        }
    }
    public class ConfigureGameObject : ConfigureSection {
        public ConfigureGameObject() : base("configure.gameobject") { }
        protected override void Process(XElement item) {
            //get parent name from parent or attribute
            string parentName=mgr.collada.nodeName;
            string parentName2=(string)item.Attribute("parent");
            if(parentName2==null) parentName2=parentName;

            //get gob name
            mgr.collada.nodeName=parentName2+"."+(string)item.Attribute("name");

            //make gob
            GameObject node=mgr.GetData<GameObject>(mgr.collada.nodeName,GetterType.Create);
            if(node==null) {
                mgr.collada.nodeName=parentName;
                return;
            }

            //add to perent
            GameObject parentNode=mgr.GetData<GameObject>(parentName2);
            if(parentNode!=null) {
                if(!parentNode.children.Contains(mgr.collada.nodeName))
                    parentNode.children.Add(mgr.collada.nodeName);
            }

            //point to node
            Node baseNode=null;
            string inherit=(string)item.Attribute("inherit");
            if(inherit!=null)
                baseNode=mgr.GetData<Node>(inherit,GetterType.Fetch);
            if(baseNode!=null)
                node.nodeData=baseNode;

            //do children and modules
            foreach(XElement child in item.Elements()) {
                if(child.Name.LocalName=="gameobject") {
                    mgr.Process("configure.gameobject",child);
                    continue;
                }
                IGOBModule module=(IGOBModule)mgr.MakeModule(child.Name.LocalName);
                if(module==null) continue;

                node.AddModule(mgr,module);
                module.Load(child);
            }

            //copy children from node
            if(baseNode!=null)
                foreach(string childName in baseNode.children)
                    AddChild(childName);

            mgr.collada.nodeName=parentName;
        }
        private void AddChild(string childName) {
            //get parent name from parent or attribute
            string parentName=mgr.collada.nodeName;

            //get gob name
            mgr.collada.nodeName=parentName+"."+childName.Split('.').Last();

            //make gob
            GameObject node=mgr.GetData<GameObject>(mgr.collada.nodeName,GetterType.Create);
            if(node==null) {
                mgr.collada.nodeName=parentName;
                return;
            }

            //add to perent
            GameObject parentNode=mgr.GetData<GameObject>(parentName,GetterType.Fetch);
            if(parentNode!=null) {
                if(!parentNode.children.Contains(mgr.collada.nodeName))
                    parentNode.children.Add(mgr.collada.nodeName);
            }

            //point to node
            Node baseNode=mgr.GetData<Node>(childName);
            if(baseNode!=null)
                node.nodeData=baseNode;

            //copy children from node
            if(baseNode!=null)
                foreach(string childName2 in baseNode.children)
                    AddChild(childName2);

            mgr.collada.nodeName=parentName;
        }
    }
    public class ProcessGameObject : ConvertItem<ConverterMgr,GameObject> {
        public ProcessGameObject() : base("process.gameobject") { }

        protected override void Process(GameObject node) {
            Node baseNode=(Node)node.nodeData;
            if(baseNode==null) return;

            if(!node.gotData) {
                IGOBModule module=null;
                switch(baseNode.dataType) {
                    case Node.DataType.node:
                    case Node.DataType.joint:
                    case Node.DataType.light:
                        module=(IGOBModule)mgr.MakeModule("dummy");
                        break;
                    case Node.DataType.camera:
                        module=(IGOBModule)mgr.MakeModule("camera");
                        break;
                    case Node.DataType.geometry:
                    case Node.DataType.controller:
                        module=(IGOBModule)mgr.MakeModule("model");
                        break;
                }
                if(module!=null) {
                    node.AddModule(mgr,module);
                    module.Load(null);
                }
            }
            if(!node.gotFrame) {
                IGOBModule module=null;
                if(baseNode.frameData.Count==0)
                    module=(IGOBModule)mgr.MakeModule("frameIdent");
                else if(IsPRS(baseNode.frameData))
                    module=(IGOBModule)mgr.MakeModule("framePRS");
                else
                    module=(IGOBModule)mgr.MakeModule("frameM");
                if(module!=null) {
                    node.AddModule(mgr,module);
                    module.Load(null);
                }
            }
        }
        private bool IsPRS(List<Node.FrameData> list) {
            int numTrans=0;
            int numRotX=0;
            int numRotY=0;
            int numRotZ=0;
            int numScale=0;

            foreach(Node.FrameData data in list) {
                switch(data.type) {
                    case Node.FrameType.rotateX:
                        numRotX++;
                        if(numRotX>1) return false;
                        break;
                    case Node.FrameType.rotateY:
                        numRotY++;
                        if(numRotY>1) return false;
                        break;
                    case Node.FrameType.rotateZ:
                        numRotZ++;
                        if(numRotZ>1) return false;
                        break;
                    case Node.FrameType.scale:
                        numScale++;
                        if(numScale>1) return false;
                        break;
                    case Node.FrameType.translate:
                        numTrans++;
                        if(numTrans>1) return false;
                        break;
                    default:
                        return false;
                }
            }
            return true;
        }
    }
    public class ModelModule : GOBModule {
        public string sceneName="";
        public string modelName="";
        public string[] matNames=null;
        public override void Load(XElement item) {
            LoadBase(item,"model",true);
            parent.gotData=true;
            Node baseNode=(Node)parent.nodeData;

            string xModel=null;
            string xMaterial=null;
            string xScene=null;
            if(item!=null) {
                xModel=mgr.configure.ReadAttrOrElem(item,"model");
                xMaterial=mgr.configure.ReadAttrOrElem(item,"material");
                xScene=(string)item.Attribute("scene");
            }
            if(xModel!=null)
                modelName=xModel;
            else if(baseNode!=null)
                modelName=baseNode.model;
            if(xMaterial!=null)
                matNames=xMaterial.Split(" ,".ToCharArray());
            else if(baseNode!=null)
                matNames=baseNode.materials.ToArray();
            if(xScene!=null)
                sceneName=xScene;
            else
                sceneName=mgr.name+".scene";
            Scene scene=mgr.GetData<Scene>(sceneName,(xScene!=null) ? GetterType.CreateAsDefault : GetterType.CreateOrDefault);
            sceneName=mgr.GetName(scene);
        }
        public override void Save(Binary binary) {
            binary.WriteReference(sceneName);
            binary.WriteReference(modelName);
            binary.bw.Write(matNames.Length);
            foreach(string mat in matNames)
                binary.WriteReference(mat);
        }
    }
    public class DummyModule : GOBModule {
        public override void Load(XElement item) {
            LoadBase(item,"dummy",true);
            parent.gotData=true;
        }
        public override void Save(Binary binary) {
        }
    }
    public class CameraModule : GOBModule {
        public string sceneName="";
        public string effectName="";
        public bool ortho=false;
        public float[] values=new float[4]{0.66f,1.5f,0.001f,10000f};
        public override void Load(XElement item) {
            LoadBase(item,"camera",true);
            parent.gotData=true;
            Node baseNode=(Node)parent.nodeData;

            XElement xPersp=null;
            XElement xOrtho=null;
            string xScene=null;
            string xEffect=null;
            if(item!=null) {
                xPersp=item.Element("perspective");
                xOrtho=item.Element("orthographic");
                xScene=(string)item.Attribute("scene");
                xEffect=(string)item.Attribute("effect");
            }
            if(xPersp!=null) {
                ortho=false;
                values[0]=(float)xPersp.Attribute("fov");
                values[1]=(float)xPersp.Attribute("aspect");
                values[2]=(float)xPersp.Attribute("nearz");
                values[3]=(float)xPersp.Attribute("farz");
            } else if(xOrtho!=null) {
                ortho=true;
                values[0]=(float)xPersp.Attribute("width");
                values[1]=(float)xPersp.Attribute("height");
                values[2]=(float)xPersp.Attribute("nearz");
                values[3]=(float)xPersp.Attribute("farz");
            } else if(baseNode!=null && baseNode.camera!=null) {
                //TODO sort out collada value combinations
                ortho=baseNode.camera.ortho;
                if(ortho) {
                    values[0]=baseNode.camera.x;
                    values[1]=baseNode.camera.y;
                } else {
                    values[0]=baseNode.camera.y;
                    values[1]=baseNode.camera.aspect;
                }
                values[2]=baseNode.camera.znear;
                values[3]=baseNode.camera.zfar;
            }
            if(xScene!=null)
                sceneName=xScene;
            else
                sceneName=mgr.name+".scene";
            Scene scene=mgr.GetData<Scene>(sceneName,(xScene!=null) ? GetterType.CreateAsDefault : GetterType.CreateOrDefault);
            sceneName=mgr.GetName(scene);
            if(xEffect!=null)
                effectName=xEffect;
            else {
                Effect eff=mgr.GetData<Effect>("",GetterType.FetchOrDefault);
                if(eff!=null)
                    effectName=mgr.GetName(eff);
            }
        }
        public override void Save(Binary binary) {
            binary.WriteReference(sceneName);
            binary.WriteReference(effectName);
            binary.WriteBool(ortho);
            binary.bw.Write(values[0]); binary.bw.Write(values[1]); binary.bw.Write(values[2]); binary.bw.Write(values[3]);
        }
    }
    public class FrameMModule : GOBModule {
        public Matrix mat;
        public override void Load(XElement item) {
            LoadBase(item,"frame",true);
            parent.gotFrame=true;
            Node baseNode=(Node)parent.nodeData;

            string xMat=null;
            if(item!=null) {
                xMat=mgr.configure.ReadAttrOrElem(item,"matrix");
            }
            if(xMat!=null) {
                float[] data=xMat.Split(' ').Select((s) => float.Parse(s)).ToArray();
                mat=new Matrix(data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11],data[12],data[13],data[14],data[15]);
            } else if(baseNode!=null)
                mat=baseNode.finalMat;
        }
        public override void Save(Binary binary) {
            binary.bw.Write(mat.M11); binary.bw.Write(mat.M12); binary.bw.Write(mat.M13); binary.bw.Write(mat.M14);
            binary.bw.Write(mat.M21); binary.bw.Write(mat.M22); binary.bw.Write(mat.M23); binary.bw.Write(mat.M24);
            binary.bw.Write(mat.M31); binary.bw.Write(mat.M32); binary.bw.Write(mat.M33); binary.bw.Write(mat.M34);
            binary.bw.Write(mat.M41); binary.bw.Write(mat.M42); binary.bw.Write(mat.M43); binary.bw.Write(mat.M44);
        }
    }
    public class FramePQSModule : GOBModule {
        public Vector3 pos=Vector3.Zero;
        public Quaternion rot=Quaternion.Identity;
        public Vector3 scale=Vector3.One;

        public override void Load(XElement item) {
            LoadBase(item,"frame",true);
            parent.gotFrame=true;
            Node baseNode=(Node)parent.nodeData;

            string xPos=null;
            string xRot=null;
            string xScale=null;
            if(item!=null) {
                xPos=mgr.configure.ReadAttrOrElem(item,"pos");
                xRot=mgr.configure.ReadAttrOrElem(item,"rot");
                xScale=mgr.configure.ReadAttrOrElem(item,"scale");
            }
            if(xPos!=null) {
                float[] data=xPos.Split(' ').Select((s) => float.Parse(s)).ToArray();
                pos=new Vector3(data[0],data[1],data[2]);
            } else if(baseNode!=null)  {
                Node.FrameData data=baseNode.frameData.FirstOrDefault((f) => f.type==Node.FrameType.translate);
                if(data!=null) pos=data.vec1;
            }
            if(xRot!=null) {
                float[] data=xRot.Split(' ').Select((s) => float.Parse(s)).ToArray();
                rot=new Quaternion(data[0],data[1],data[2],data[3]);
            } else if(baseNode!=null) {
                Vector3 euler=Vector3.Zero;
                Node.FrameData x=baseNode.frameData.FirstOrDefault((f) => f.type==Node.FrameType.rotateX);
                Node.FrameData y=baseNode.frameData.FirstOrDefault((f) => f.type==Node.FrameType.rotateY);
                Node.FrameData z=baseNode.frameData.FirstOrDefault((f) => f.type==Node.FrameType.rotateZ);
                if(x!=null) euler.X=x.value;
                if(y!=null) euler.Y=y.value;
                if(z!=null) euler.Z=z.value;
                rot=Quaternion.CreateFromYawPitchRoll(euler.Y,euler.X,euler.Z);
            }
            if(xScale!=null) {
                float[] data=xScale.Split(' ').Select((s) => float.Parse(s)).ToArray();
                scale=new Vector3(data[0],data[1],data[2]);
            } else if(baseNode!=null) {
                Node.FrameData data=baseNode.frameData.FirstOrDefault((f) => f.type==Node.FrameType.scale);
                if(data!=null) scale=data.vec1;
            }
        }
        public override void Save(Binary binary) {
            binary.bw.Write(pos.X); binary.bw.Write(pos.Y); binary.bw.Write(pos.Z);
            binary.bw.Write(rot.X); binary.bw.Write(rot.Y); binary.bw.Write(rot.Z); binary.bw.Write(rot.W);
            binary.bw.Write(scale.X); binary.bw.Write(scale.Y); binary.bw.Write(scale.Z);
        }
    }
    public class FramePRSModule : GOBModule {
        public Vector3 pos=Vector3.Zero;
        public Vector3 rot=Vector3.Zero;
        public Vector3 scale=Vector3.One;

        public override void Load(XElement item) {
            LoadBase(item,"frame",true);
            parent.gotFrame=true;
            Node baseNode=(Node)parent.nodeData;

            string xPos=null;
            string xRot=null;
            string xScale=null;
            if(item!=null) {
                xPos=mgr.configure.ReadAttrOrElem(item,"pos");
                xRot=mgr.configure.ReadAttrOrElem(item,"rot");
                xScale=mgr.configure.ReadAttrOrElem(item,"scale");
            }
            if(xPos!=null) {
                float[] data=xPos.Split(' ').Select((s) => float.Parse(s)).ToArray();
                pos=new Vector3(data[0],data[1],data[2]);
            } else if(baseNode!=null) {
                Node.FrameData data=baseNode.frameData.FirstOrDefault((f) => f.type==Node.FrameType.translate);
                if(data!=null) pos=data.vec1;
            }
            if(xRot!=null) {
                float[] data=xRot.Split(' ').Select((s) => float.Parse(s)).ToArray();
                rot=new Vector3(data[0],data[1],data[2]);
            } else if(baseNode!=null) {
                Node.FrameData x=baseNode.frameData.FirstOrDefault((f) => f.type==Node.FrameType.rotateX);
                Node.FrameData y=baseNode.frameData.FirstOrDefault((f) => f.type==Node.FrameType.rotateY);
                Node.FrameData z=baseNode.frameData.FirstOrDefault((f) => f.type==Node.FrameType.rotateZ);
                if(x!=null) rot.X=x.value;
                if(y!=null) rot.Y=y.value;
                if(z!=null) rot.Z=z.value;
            }
            if(xScale!=null) {
                float[] data=xScale.Split(' ').Select((s) => float.Parse(s)).ToArray();
                scale=new Vector3(data[0],data[1],data[2]);
            } else if(baseNode!=null) {
                Node.FrameData data=baseNode.frameData.FirstOrDefault((f) => f.type==Node.FrameType.scale);
                if(data!=null) scale=data.vec1;
            }
        }
        public override void Save(Binary binary) {
            binary.bw.Write(pos.X); binary.bw.Write(pos.Y); binary.bw.Write(pos.Z);
            binary.bw.Write(rot.X); binary.bw.Write(rot.Y); binary.bw.Write(rot.Z);
            binary.bw.Write(scale.X); binary.bw.Write(scale.Y); binary.bw.Write(scale.Z);
        }
    }
    public class FrameIdentModule : GOBModule {
        public override void Load(XElement item) {
            LoadBase(item,"frame",true);
            parent.gotFrame=true;
        }
        public override void Save(Binary binary) {
        }
    }
    internal class GameObjectBinaryWriter : ConvertItem<HierarchyModule,GameObject> {
        public GameObjectBinaryWriter() : base("write.gameobject") { }
        protected override void Process(GameObject item) {
            parent.binary.OpenFile("GOB ",mgr.GetName(item));
            
            parent.binary.bw.Write((uint)item.children.Count);
            parent.binary.bw.Write((uint)item.modules.Count);

            foreach(string child in item.children)
                parent.binary.WriteReference(child);

            foreach(IGOBModule module in item.modules) {
                parent.binary.OpenFile("MODL");
                parent.binary.WriteString(module.name);
                parent.binary.WriteBool(module.enableOnInit);
                parent.binary.WriteString(module.type);
                module.Save(parent.binary);
                parent.binary.CloseFile();
            }

            parent.binary.CloseFile();
        }
    }
    public class NodeTextWriter : ConvertItem<TextHierModule,Node> {
        public NodeTextWriter() : base("write.node") { }
        protected override void Process(Node item) {
            parent.lines.Add(string.Format("node {0} {1}",mgr.GetName(item),item.dataType.ToString()));

            foreach(Node.FrameData data in item.frameData) {
                switch(data.type) {
                    case Node.FrameType.lookat:
                        parent.lines.Add(string.Format("{0} {1} {2} {3} {4}",data.type.ToString(),data.name,data.vec1.ToString(),data.vec2.ToString(),data.vec3.ToString()));
                        break;
                    case Node.FrameType.matrix:
                        parent.lines.Add(string.Format("{0} {1} {2}",data.type.ToString(),data.name,data.mat.ToString()));
                        break;
                    case Node.FrameType.rotate:
                    case Node.FrameType.rotateX:
                    case Node.FrameType.rotateY:
                    case Node.FrameType.rotateZ:
                        parent.lines.Add(string.Format("{0} {1} {2} {3}",data.type.ToString(),data.name,data.vec1.ToString(),data.value));
                        break;
                    case Node.FrameType.scale:
                    case Node.FrameType.translate:
                        parent.lines.Add(string.Format("{0} {1} {2}",data.type.ToString(),data.name,data.vec1.ToString()));
                        break;
                    case Node.FrameType.skew:
                        parent.lines.Add(string.Format("{0} {1} {2} {3} {4}",data.type.ToString(),data.name,data.vec1.ToString(),data.name,data.vec2.ToString(),data.value));
                        break;
                }
            }

            if(item.model!="")
                parent.lines.Add(string.Format("model {0}",item.model));
            foreach(string mat in item.materials)
                parent.lines.Add(string.Format("material {0}",mat));

            if(item.camera!=null)
                parent.lines.Add(string.Format("camera {0} {1} {2} {3} {4} {5}",
                    item.camera.ortho.ToString(),item.camera.x,item.camera.y,item.camera.aspect,item.camera.znear,item.camera.zfar));

            foreach(string child in item.children)
                parent.lines.Add(string.Format("child {0}",child));
            parent.lines.Add("");
        }
    }
}
