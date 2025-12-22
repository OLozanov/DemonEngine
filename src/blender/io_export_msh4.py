bl_info = {
    "name":     "MSH4 Exporter",
    "author":   "Losanov Alexey",
    "blender":  (3,0,0),
    "version":  (1,0,0),
    "location": "File > Import-Export",
    "description":  "Exports geometry for Demon Engine 2",
    "category": "Import-Export"
}

import bpy
from bpy.props import (
        BoolProperty,
        IntProperty,
        FloatProperty,
        )
import math
from array import *
from struct import pack
from bpy_extras.io_utils import ExportHelper
from mathutils import *

class TriangleWrapper(object):
    __slots__ = "vertex_indices", "offset";
    
    def __init__(self, vertex_index=(0,0,0)):
        self.vertex_indices = vertex_index;

class TSpace(object):
    __slots__ = "norm", "tangent", "binorm", "num";
    
    def __init__(self):
        self.norm = (0,0,0);
        self.tangent = (0,0,0);
        self.binorm = (0,0,0);
        self.num = 0;
        
class Vertex(object):
    __slots__ = "vert", "uv", "norm", "tangent", "binorm", "vid", "idx", "uvid", "bindex";

    def __init__(self, coord=(0,0,0), tcoord=(0,0), vnum=0, id=0,  tnum=0):
        self.vert = coord;
        self.uv = tcoord;
        self.vid = vnum;
        self.idx = id;
        self.uvid = tnum;
        self.norm = (0,0,0);
        self.tangent = (0,0,0);
        self.binorm = (0,0,0);
        self.bindex = 0;
        
    def write(self, scale, file):  
        file.write(pack('fff', self.vert[0]*scale, self.vert[1]*scale, self.vert[2]*scale));
        file.write(pack('ff', self.uv[0], self.uv[1]));
        file.write(pack('fff', self.norm[0], self.norm[1], self.norm[2]));
        file.write(pack('fff', self.tangent[0], self.tangent[1], self.tangent[2]));
        file.write(pack('fff', self.binorm[0], self.binorm[1], self.binorm[2]));
        
    def writeBoneId(self, file):
        file.write(pack('l', self.bindex));
        
class VertexRef(object):
    __slots__ = "idx", "uv_idx", "vid";
    
    def __init__(self, id, tid):
        self.idx = id;
        self.uv_idx = tid;
        self.vid = 0;
    
class VertexSet(object):
    __slots__ = "mesh", "vertices", "entries", "ptr";
    
    def __init__(self, mesh):
        self.mesh = mesh;
        self.vertices = [];
        self.entries = [[] for i in range(len(mesh.vertices))];
        
    def getVertex(self, idx, uv_idx, norm, tangent, binorm, smooth):
        
        for i in self.entries[idx]:
            vert = self.vertices[i];
        
            if smooth: 
                if vert.uvid == uv_idx:
                    return i;
            else:
                ndot = norm[0] * vert.norm[0] + norm[1] * vert.norm[1] + norm[2] * vert.norm[2];
                tdot = tangent[0] * vert.tangent[0] + tangent[1] * vert.tangent[1] + tangent[2] * vert.tangent[2];
                bdot = binorm[0] * vert.binorm[0] + binorm[1] * vert.binorm[1] + binorm[2] * vert.binorm[2];
            
                if vert.uvid == uv_idx and math.isclose(ndot, 1.0) and math.isclose(tdot, 1.0) and math.isclose(bdot, 1.0):
                    return i;
                
        vid = len(self.vertices);
        
        mvert = self.mesh.vertices[idx];
        mtex = self.mesh.uv_layers.active.data[uv_idx];
        
        pos = (-mvert.co[0], mvert.co[2], -mvert.co[1]);
        tcoord = (mtex.uv[0], -mtex.uv[1]);
        
        self.vertices.append(Vertex(pos, tcoord, vid, idx, uv_idx));
        
        vert = self.vertices[-1];
        vert.norm = norm;
        vert.tangent = tangent;
        vert.binorm = binorm;
        
        self.entries[idx].append(vid);
        
        return vid;
        
    def __iter__(self):
        self.ptr = 0
        return self

    def __next__(self):
        if self.ptr < len(self.vertices):
            result = self.vertices[self.ptr];
            self.ptr += 1;
            return result;
        else:
            raise StopIteration;

class Triangle(object):
    __slots__ = "v", "smooth", "norm", "tangent", "binorm";

    def __init__(self, v1, v2, v3, sm):
        self.v = (v1, v2, v3);
        self.smooth = sm;
        self.norm = (0,0,0);
        self.tangent = (0,0,0);
        self.binorm = (0,0,0);
        
    def culcTSpace(self, mesh):
        uvlayer = mesh.uv_layers.active;
    
        vert1 = mesh.vertices[self.v[0].idx];
        vert2 = mesh.vertices[self.v[1].idx];
        vert3 = mesh.vertices[self.v[2].idx];

        tcoord1 = uvlayer.data[self.v[0].uv_idx];
        tcoord2 = uvlayer.data[self.v[1].uv_idx];
        tcoord3 = uvlayer.data[self.v[2].uv_idx];
    
        v1 = (-vert1.co[0], vert1.co[2], -vert1.co[1]);
        v2 = (-vert2.co[0], vert2.co[2], -vert2.co[1]);
        v3 = (-vert3.co[0], vert3.co[2], -vert3.co[1]);

        tv1 = (tcoord1.uv[0], -tcoord1.uv[1])
        tv2 = (tcoord2.uv[0], -tcoord2.uv[1])
        tv3 = (tcoord3.uv[0], -tcoord3.uv[1])

        #triangle edges
        l1 = (v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2]);
        l2 = (v3[0] - v1[0], v3[1] - v1[1], v3[2] - v1[2]);

        norm = cross(l1, l2);           

        #texture coords
        s1 = tv2[0] - tv1[0];
        t1 = tv2[1] - tv1[1];

        s2 = tv3[0] - tv1[0];
        t2 = tv3[1] - tv1[1];

        det = (s1*t2 - s2*t1);
        
        if det == 0:
            
            tangent = (l1[0], l1[1], l1[2]);
            binorm = (l2[0], l2[1], l2[2]);
            
        else:
            inv_det = 1.0/(s1*t2 - s2*t1);

            tx = inv_det*(t2*l1[0] - t1*l2[0]);
            ty = inv_det*(t2*l1[1] - t1*l2[1]);
            tz = inv_det*(t2*l1[2] - t1*l2[2]);

            bx = inv_det*(s2*l1[0] - s1*l2[0]);
            by = inv_det*(s2*l1[1] - s1*l2[1]);
            bz = inv_det*(s2*l1[2] - s1*l2[2]);

            tangent = (tx, ty, tz);
            binorm = (bx, by, bz);

        self.norm = normalize(norm);
        self.tangent = normalize(tangent);
        self.binorm = normalize(binorm);

class DCBone(object):
    
    __slots__ = "num", "bone", "parent", "cfirst", "clast", "mat", "wmat", "constraint_id";

    def __init__(self, num, bone, mat, wmat):
        self.num = num;
        self.bone = bone;
        self.parent = -1;
        self.cfirst = -1;
        self.clast = -1;
        self.mat = mat;
        self.wmat = wmat;
        self.constraint_id = -1;

class DCArmature(object):

    __slots__ = "root", "mat", "pos";
    
    def __init__(self, dcbone, mat, pos):
        self.root = dcbone.num;
        self.mat = mat;
        self.pos = pos;

    def save(self, file):
        file.write(pack('L', self.root));
        save_mat3(self.mat, file);
        file.write(pack('fff', self.pos[0], self.pos[1], self.pos[2]));
        #file.write(pack('fff', self.rot[0], self.rot[1], self.rot[2]));

class DCMesh(object):
    
    __slots__ = "vnum", "offset", "texture", "bl_obj"
    
    def __init__(self, vnum, offset, tname, obj):
        self.vnum = vnum;
        self.offset = offset;
        self.texture = tname;
        self.bl_obj = obj;

    def save(self, file):
        file.write(pack('L', self.vnum));
        file.write(pack('L', self.offset));
        save_str(file, self.texture);
        
class DCObject(object):

    __slots__ = "obj", "meshnum"
    
    def __init__(self, obj, meshnum):
        self.obj = obj;
        self.meshnum = meshnum
        
    def save(self, file):
        file.write(pack('H', self.meshnum));
   

class DCMExport(bpy.types.Operator, ExportHelper):
    bl_idname = "export_dcmesh.msh";
    bl_label = "MSH4 Exporter";
    bl_options = {'PRESET'};

    filename_ext = ".msh";
    
    scale: FloatProperty(
        name="Scale",
        description="Geometry scale",
        min=0.001, max=1000.0,
        soft_min=0.01, soft_max=1000.0,
        default=1.0,
        )
    export_animation: BoolProperty(
            name="Export Animation",
            description="Export animation data",
            default=True,
            )
    frame_step: IntProperty(
        name="Frame step",
        description="Interval between saved frames",
        min=0, max=100,
        soft_min=0, soft_max=100,
        default=5,
        )
    
    __slots__ = "dcmeshes", "dcobjects", "dcbones", "bone_constraints", "vertices", "triangles", "bmap", "bindex", "binum", "armatures", "collision_polygons", "anum", "glmat";         
    
    def buildVertices(self, obj, mesh, triangles):
        offset = len(self.vertices)
    
        vertices = VertexSet(mesh);
        tspace = {};

        vlen = len(mesh.vertices);

        #for v in range(vlen):
        #    tspace[v] = TSpace();

        for tri in triangles:
            if tri.smooth:
                i1 = tri.v[0].idx;
                i2 = tri.v[1].idx;
                i3 = tri.v[2].idx;
                
                if not i1 in tspace:
                    tspace[i1] = TSpace();
                    
                if not i2 in tspace:
                    tspace[i2] = TSpace();

                if not i3 in tspace:
                    tspace[i3] = TSpace();                    

                tspace[i1].norm = vadd(tspace[i1].norm, tri.norm);
                tspace[i2].norm = vadd(tspace[i2].norm, tri.norm);
                tspace[i3].norm = vadd(tspace[i3].norm, tri.norm);

                tspace[i1].tangent = vadd(tspace[i1].tangent, tri.tangent);
                tspace[i2].tangent = vadd(tspace[i2].tangent, tri.tangent);
                tspace[i3].tangent = vadd(tspace[i3].tangent, tri.tangent);

                tspace[i1].binorm = vadd(tspace[i1].binorm, tri.binorm);
                tspace[i2].binorm = vadd(tspace[i2].binorm, tri.binorm);
                tspace[i3].binorm = vadd(tspace[i3].binorm, tri.binorm);

                tspace[i1].num += 1; 
                tspace[i2].num += 1; 
                tspace[i3].num += 1; 

        #normalize vectors
        for vert in tspace.values():

            if vert.num > 0:
                norm = (vert.norm[0]/vert.num, vert.norm[1]/vert.num, vert.norm[2]/vert.num);
                tangent = (vert.tangent[0]/vert.num, vert.tangent[1]/vert.num, vert.tangent[2]/vert.num);
                binorm = (vert.binorm[0]/vert.num, vert.binorm[1]/vert.num, vert.binorm[2]/vert.num);

                vert.norm = normalize(norm);
                vert.tangent = normalize(tangent);
                vert.binorm = normalize(binorm);

        for tri in triangles:
            i1 = tri.v[0].idx;
            i2 = tri.v[1].idx;
            i3 = tri.v[2].idx;
            
            t1 = tri.v[0].uv_idx;
            t2 = tri.v[1].uv_idx;
            t3 = tri.v[2].uv_idx;
        
            if tri.smooth:               
                tri.v[0].vid = vertices.getVertex(i1, t1, tspace[i1].norm, tspace[i1].tangent, tspace[i1].binorm, True) + offset;
                tri.v[1].vid = vertices.getVertex(i2, t2, tspace[i2].norm, tspace[i2].tangent, tspace[i2].binorm, True) + offset;
                tri.v[2].vid = vertices.getVertex(i3, t3, tspace[i3].norm, tspace[i3].tangent, tspace[i3].binorm, True) + offset;
            else:
                tri.v[0].vid = vertices.getVertex(i1, t1, tri.norm, tri.tangent, tri.binorm, False) + offset;
                tri.v[1].vid = vertices.getVertex(i2, t2, tri.norm, tri.tangent, tri.binorm, False) + offset;
                tri.v[2].vid = vertices.getVertex(i3, t3, tri.norm, tri.tangent, tri.binorm, False) + offset;
                
        for vert in vertices:
            mvert = mesh.vertices[vert.idx];
            vert.bindex = self.getBoneIndex(obj, mvert.groups);
            
            self.vertices.append(vert);
        
    def getBoneIndex(self, object, vgroups):
        
        gid = 0;
        
        if len(vgroups) > 0:
            gid = vgroups[0].group;
        else: 
            return -1;
        
        group = object.vertex_groups[gid];
        bname = group.name;
        
        if bname in self.bmap:
            dcbone = self.bmap[bname];
            return dcbone.num;
                
        return -1;
        
    def addSubmesh(self, triangles, indices, texture, obj):
        
        vnum = len(indices);
        offset = len(self.triangles);
        
        for i in indices:
            self.triangles.append(triangles[i]);
            
        dcmesh = DCMesh(vnum, offset, texture, obj);
        self.dcmeshes.append(dcmesh);
        
    def addMesh(self, obj, mesh):
        triangles = [];
        uvlayer = mesh.uv_layers.active;
    
        meshes = {};
    
        f = 0;      #face number
        t = 0;      #triangle number
    
        for face in mesh.loop_triangles:
            ind = face.vertices;
            loops = face.loops;
            
            tname = mesh.materials[face.material_index].name; #mesh.uv_layers.active.data[f].image.name;
            f += 1;

            if(meshes.get(tname) is None):
                meshes[tname] = [];

            vert1 = mesh.vertices[ind[2]];
            vert2 = mesh.vertices[ind[1]];
            vert3 = mesh.vertices[ind[0]];

            tcoord1 = uvlayer.data[loops[2]];
            tcoord2 = uvlayer.data[loops[1]];
            tcoord3 = uvlayer.data[loops[0]];
            
            a = VertexRef(ind[2], loops[2]);
            b = VertexRef(ind[1], loops[1]);
            c = VertexRef(ind[0], loops[0]);
            
            new_tri = Triangle(a, b, c, face.use_smooth);
            new_tri.culcTSpace(mesh);

            triangles.append(new_tri);
            
            meshes[tname].append(t);
            t += 1;
        
        self.buildVertices(obj, mesh, triangles);     
    
        for tex in meshes:
            indices = meshes[tex];
            self.addSubmesh(triangles, indices, tex, obj);
        
        dcobj = DCObject(obj, len(meshes));
        self.dcobjects.append(dcobj);

    def addCollisionMesh(self, mesh):
        for face in mesh.polygons:
            verts = face.vertices;
            start = face.loop_start;
            
            polygon = [];
                    
            v = 0;
            while v < face.loop_total:
                ind = mesh.loops[start+v].vertex_index;
                vert = mesh.vertices[ind];
                glvert = self.glmat@Vector((vert.co[0], vert.co[1], vert.co[2]));           
                
                polygon.append(glvert);
                
                v = v + 1;
        
            self.collision_polygons.append(polygon);
        
    def addbone(self, bone, mat, wmat):
   
        bid = len(self.dcbones);
        
        dcbone = DCBone(bid, bone, mat, wmat);
        self.dcbones.append(dcbone);

        self.bmap[bone.name] = dcbone;

        for constraint in bone.constraints:
  
            if constraint.enabled and constraint.type == 'LIMIT_ROTATION':
                if constraint.use_limit_x or constraint.use_limit_y or constraint.use_limit_z:
                    dcbone.constraint_id = len(self.bone_constraints);
                    self.bone_constraints.append(constraint);
                
        return dcbone;
        
    def addbonechildren(self, dcbone, mat, wmat):

        bone = dcbone.bone;
    
        children = [];
        
        cmat = mat*bone.bone.matrix;
        
        for child in bone.children:

            child_id = len(self.dcbones);
            
            dcchild = self.addbone(child, cmat, wmat);
            children.append(dcchild);
            
            dcchild.parent = dcbone.num;
               
            if dcbone.cfirst == -1:
                dcbone.cfirst = dcchild.num;
            
            dcbone.clast = dcchild.num;
                
        for dcchild in children:
            self.addbonechildren(dcchild, mat, wmat);
    
    def addArmature(self, armature):

        root = armature.pose.bones[0];
            
        for bone in armature.pose.bones:
            if bone.parent is None:
                root = bone;
        
        mat = Matrix();
        mat.identity();
        
        wmat = armature.matrix_basis;
        
        dcbone = self.addbone(root, mat.to_3x3(), wmat);
        self.addbonechildren(dcbone, mat.to_3x3(), wmat);
        
        mat = self.glmat@armature.matrix_basis;
        #rot = mat.to_euler('XYZ');
        pos = armature.location + root.bone.head;
        
        dcarm = DCArmature(dcbone, mat, (self.glmat@pos)*self.scale);
        self.armatures.append(dcarm);
        self.anum += 1;

    def savebroots(self, file):
        
        for arm in self.armatures:
            arm.save(file);
    
    def savebone(self, dcbone, file):
        bone = dcbone.bone;
        
        head = (0, 0, 0); #bone.bone.head;
        #tail = bone.bone.tail_local - bone.bone.head_local;
        #len = tail.length;       
       
        #rhead = self.glmat@Vector((head[0], head[1], head[2]));
        #rtail = self.glmat@Vector((tail[0], tail[1], tail[2]));
            
        #file.write(pack('fff', rhead[0]*self.scale, rhead[1]*self.scale, rhead[2]*self.scale));
        #file.write(pack('fff', rtail[0]*self.scale, rtail[1]*self.scale, rtail[2]*self.scale));
        file.write(pack('fhhhh', bone.bone.length*self.scale, dcbone.parent, dcbone.cfirst, dcbone.clast, dcbone.constraint_id));    
    
    def savebone_matrix(self, dcbone, file):
        bone = dcbone.bone.bone;
        
        mat = self.glmat@bone.matrix_local;
        
        yaxis = Vector((mat[0][1], mat[1][1], mat[2][1]));
        zaxis = Vector((mat[0][2], mat[1][2], mat[2][2]));
        xaxis = yaxis.cross(zaxis);
        
        #file.write(pack('fff', mat[0][0], mat[1][0], mat[2][0]));
        #file.write(pack('fff', mat[0][1], mat[1][1], mat[2][1]));
        #file.write(pack('fff', mat[0][2], mat[1][2], mat[2][2]));
        
        file.write(pack('fff', xaxis[0], xaxis[1], xaxis[2]));
        file.write(pack('fff', yaxis[0], yaxis[1], yaxis[2]));
        file.write(pack('fff', zaxis[0], zaxis[1], zaxis[2]));
        file.write(pack('fff', mat[0][3]*self.scale, mat[1][3]*self.scale, mat[2][3]*self.scale));
                
    def saveconstraint(self, constraint, file):       
        min = [float('inf'), float('inf'), float('inf')];
        max = [float('-inf'), float('-inf'), float('-inf')];
                
        if constraint.use_limit_x:
            min[0] = constraint.min_x;
            max[0] = constraint.max_x;
            
        if constraint.use_limit_y:
            min[1] = constraint.min_y;
            max[1] = constraint.max_y;
            
        if constraint.use_limit_z:
            min[2] = -constraint.max_z;
            max[2] = -constraint.min_z;
            
        file.write(pack('fff', min[0], min[1], min[2]));
        file.write(pack('fff', max[0], max[1], max[2]));        
        
    def savebone_anim(self, dcbone, file):
        bone = dcbone.bone;
            
        quat = bone.rotation_quaternion;
        rot = quat.to_euler('XYZ');
        file.write(pack('fff', rot[0], -rot[1], -rot[2]));
               
    def savebone_anim_pos(self, dcbone, file):
        bone = dcbone.bone;
        
        pos = (self.glmat@bone.location)*self.scale;
        file.write(pack('fff', pos[0], pos[1], -pos[2]));

    def save_vertices(self, file):
    
        for vert in self.vertices:
            vert.write(self.scale, file);

        if self.dcbones:
            for vert in self.vertices:
                vert.writeBoneId(file);
    
    def save_triangles(self, file):    
    
        for tri in self.triangles:          
            file.write(pack('HHH', tri.v[0].vid, tri.v[1].vid, tri.v[2].vid));
        
    def saveCollisionPolygon(self, polygon, scale, file):
        file.write(pack('H', len(polygon)));
        
        for vert in reversed(polygon):
            file.write(pack('fff', scale*vert[0], scale*vert[1], scale*vert[2]));
    
    def saveCollisionGeometry(self, scale, file):
        
        for polygon in self.collision_polygons:
            self.saveCollisionPolygon(polygon, scale, file);
    
    def glmat_init(self):
        self.glmat = Matrix();
        
        #OpenGl world matrix
        self.glmat[0][0], self.glmat[0][1], self.glmat[0][2], self.glmat[0][3] = -1.0, 0.0, 0.0, 0.0;
        self.glmat[1][0], self.glmat[1][1], self.glmat[1][2], self.glmat[0][3] = 0.0, 0.0, 1.0, 0.0;
        self.glmat[2][0], self.glmat[2][1], self.glmat[2][2], self.glmat[0][3] = 0.0, -1.0, 0.0, 0.0;    
        self.glmat[3][0], self.glmat[3][1], self.glmat[3][2], self.glmat[3][3] = 0.0, 0.0, 0.0, 1.0;
        
    def execute(self, context):
    
        self.glmat_init();
        #self.glmat.identity();
    
        file = open(self.filepath, 'bw');
        file.write(pack('i',0x3448534D)); #signature

        scene = bpy.context.scene;
        
        start = scene.frame_start;
        end = scene.frame_end;
        
        num_frames = 0;
        
        if self.export_animation:
            num_frames = int((end - start)/self.frame_step) + 1;
            
        #meshes = [];
        bones = [];

        self.dcmeshes = [];
        self.dcobjects = [];
        self.vertices = [];
        self.triangles = [];
        
        self.dcbones = [];
        self.binum = 0;
        self.bmap = {};
        
        self.bone_constraints = [];
        
        self.bindex = [];
        self.binum = 0;
        
        self.armatures = [];
        self.anum = 0;
        
        self.collision_polygons = [];
        
        for obj in scene.objects:        
            if obj.type == 'ARMATURE':
                #self.addbone(obj.pose.bones[0]);
                self.addArmature(obj); 
        
        for obj in scene.objects:
            if obj.type == 'MESH':          
                mesh = obj.to_mesh();
                
                if obj.display_type == "WIRE":
                    self.addCollisionMesh(mesh);
                else:   
                    self.addMesh(obj, mesh);             

        file.write(pack('LLHHHHHHHH', len(self.vertices), len(self.triangles), len(self.dcmeshes), len(self.dcobjects), len(self.dcbones), num_frames, len(self.bone_constraints), self.anum, len(self.collision_polygons), 0));

        self.save_vertices(file);
        self.save_triangles(file);
        
        scene.frame_set(0);
        for dcmesh in self.dcmeshes:
            dcmesh.save(file);
            
        for dcobj in self.dcobjects:
            dcobj.save(file);
        
        for dcbone in self.dcbones:
            self.savebone(dcbone, file);
            
        for dcbone in self.dcbones:
            self.savebone_matrix(dcbone, file);
            
        for constraint in self.bone_constraints:
            self.saveconstraint(constraint, file);

        self.savebroots(file);
            
        #save objects animation data
        if self.export_animation:
            frame = start;
            
            while frame <= end:
                scene.frame_set(frame);    
                
                for dcobj in self.dcobjects:    
                    obj = dcobj.obj;
                    pos = self.glmat@obj.location;
                    rot = obj.rotation_euler;
                    hide = obj.hide_viewport;

                    file.write(pack('L', hide));
                    file.write(pack('fff', pos[0]*self.scale, pos[1]*self.scale, pos[2]*self.scale));
                    file.write(pack('fff', rot[0], rot[2], rot[1]));

                frame = frame + self.frame_step; 
            
            #save bones animation data
            frame = start;
            
            # root bone pos
            while frame <= end:
                scene.frame_set(frame); 
                
                for arm in self.armatures:
                    dcbone = self.dcbones[arm.root];
                    
                    self.savebone_anim_pos(dcbone, file);
                    
                frame = frame + self.frame_step; 
            
            frame = start;
            
            # bone rot
            while frame <= end:
                scene.frame_set(frame);         

                for dcbone in self.dcbones:
                    self.savebone_anim(dcbone, file);

                frame = frame + self.frame_step; 
        
        # Collision
        if self.collision_polygons:
            self.saveCollisionGeometry(self.scale, file);
            
        file.close();
        return {'FINISHED'};

#Vector func
def normalize(vec):
    len = math.sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);

    if len != 0:
        x = vec[0]/len;
        y = vec[1]/len;
        z = vec[2]/len;
    else:
        x = 0;
        y = 0;
        z = 0;
        
    return (x, y, z);

def dot(a, b):
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];


def cross(a, b):
    x = a[1]*b[2] - a[2]*b[1];
    y = a[2]*b[0] - a[0]*b[2];
    z = a[0]*b[1] - a[1]*b[0];

    return (x, y, z);

def vadd(a, b):
    x = a[0] + b[0];
    y = a[1] + b[1];
    z = a[2] + b[2];

    return (x, y, z);   


def save_str(file, string):
    slen = len(string);

    file.write(pack('H', slen));
    file.write(string.encode('utf-8'));

def save_mat3(mat, file):
    file.write(pack('ffff', mat[0][0], mat[1][0], mat[2][0], 0));
    file.write(pack('ffff', mat[0][1], mat[1][1], mat[2][1], 0));
    file.write(pack('ffff', mat[0][2], mat[1][2], mat[2][2], 0));
    file.write(pack('ffff', 0, 0, 0, 1));
    
def save_mat4(mat, file):
    file.write(pack('ffff', mat[0][0], mat[1][0], mat[2][0], mat[3][0]));
    file.write(pack('ffff', mat[0][1], mat[1][1], mat[2][1], mat[3][1]));
    file.write(pack('ffff', mat[0][2], mat[1][2], mat[2][2], mat[3][2]));
    file.write(pack('ffff', mat[0][3], mat[1][3], mat[2][3], mat[3][3]));   

def menu_func(self, context):
    self.layout.operator(DCMExport.bl_idname, text="Msh4(.msh)");

def register():
    bpy.utils.register_class(DCMExport);
    bpy.types.TOPBAR_MT_file_export.append(menu_func);

def unregister():
    bpy.utils.unregister_class(DCMExport);
    bpy.types.TOPBAR_MT_file_export.remove(menu_func);

if __name__ == "__main__":
    register()