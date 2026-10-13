/* Generated from frozen official Skills/Skilldesc; do not hand-edit rows. */
#include "dm_touch_native_art.h"
#include "../../host/dm_input.h"
#include <stddef.h>
#include "../../host/dm_derived_rt.h"
const dm_ta_atlas dm_ta_atlases[DM_TA_ATLASES]={
 {"dm_native_skill_0",4080,65,65,68,60},
 {"dm_native_skill_1",4080,65,65,68,60},
 {"dm_native_skill_2",4080,65,65,68,60},
 {"dm_native_skill_3",4080,65,65,68,60},
 {"dm_native_skill_4",4080,65,65,68,60},
 {"dm_native_skill_5",4080,65,65,68,60},
 {"dm_native_skill_6",4080,65,65,68,60},
 {"dm_native_skill_7",4080,65,65,68,60},
 {"dm_native_skill_8",2720,65,65,68,40},
};
const char dm_ta_manifest_sha[]="acf4edc4caef33f98bd1daeac0316981f9dee5046537da9ffb3cb1cb7a27294a";
const char dm_ta_geometry_sha[]="68cbae42c860b0252d4bbb4098788f9ea957ff29c75c5eda690bf2c619acf550";
const char dm_ta_binding_sha[]="f89225ba353ddbfe31d0e8178d96ffa862c76179a2be07b1c1c1165ce6eb8ca1";
static struct { uint32_t id; int atlas,frame; } rows[429];   /* 壳的替身：技能索引不编进代码，启动时从安装包的 dm/derived.tsv 读（标签 TAROW） */
static void ta_row_cb(char **f, int n, void *ctx) { int *i = ctx; if (n < 3 || *i >= (int)(sizeof rows / sizeof rows[0])) return;
    rows[*i].id = (uint32_t)strtoul(f[0], NULL, 10); rows[*i].atlas = atoi(f[1]); rows[*i].frame = atoi(f[2]); (*i)++; }
__attribute__((constructor)) static void ta_load(void) { int i = 0; dm_derived_each("TAROW", ta_row_cb, &i);
    for (int k = i; k < (int)(sizeof rows / sizeof rows[0]); k++) { rows[k].id = UINT32_MAX - 1; rows[k].atlas = rows[k].frame = -1; }
    if (i != (int)(sizeof rows / sizeof rows[0])) fprintf(stderr, "[dm] 【红】技能图标索引没读全：%d / %d\n", i, (int)(sizeof rows / sizeof rows[0])); }
static dm_ta_glyph unknown(uint32_t id,const char *reason) {
    return (dm_ta_glyph){DM_TA_UNKNOWN,-1,-1,-1,-1,-1,id,reason};
}
int dm_ta_frame_at(int atlas,int frame,dm_ta_frame *out) {
    if(!out || atlas<0 || atlas>=DM_TA_ATLASES || frame<0)return 0;
    const dm_ta_atlas *a=&dm_ta_atlases[atlas];
    if(frame>=a->count || frame*a->pitch+a->frame_width>a->width)return 0;
    *out=(dm_ta_frame){frame*a->pitch,0,a->frame_width,a->height};return 1;
}
dm_ta_glyph dm_ta_skill(uint32_t id) {
    dm_ta_glyph g=unknown(id,"unknown_skill");
    if(id==UINT32_MAX){g.status=DM_TA_EMPTY;g.reason="empty_slot";return g;}
    for(size_t i=0;i<sizeof rows/sizeof rows[0];i++)if(rows[i].id==id) {
        dm_ta_frame frame;
        if(!dm_ta_frame_at(rows[i].atlas,rows[i].frame,&frame) || !dm_ta_frame_at(rows[i].atlas,rows[i].frame+1,&frame))return g;
        g.status=DM_TA_GLYPH;g.atlas=rows[i].atlas;g.normal_frame=rows[i].frame;g.pressed_frame=rows[i].frame+1;g.reason="official_raw_id";return g;
    }
    return g;
}
dm_ta_glyph dm_ta_current(const dm_tg_snapshot *s,uint64_t button,uint64_t now) { return dm_ta_current_set(s,button,now,0); }
dm_ta_glyph dm_ta_current_set(const dm_tg_snapshot *s,uint64_t button,uint64_t now,int skill_set) {
    dm_ta_glyph g=unknown(UINT32_MAX,"empty_unknown");
    if(!s || !s->supported || !s->valid || !s->ready || s->source!=DM_TG_SOURCE_NATIVE_GETTER ||
       s->identity_scope!=DM_TG_IDENTITY_NATIVE || !s->captured_ns || now<s->captured_ns ||
       now-s->captured_ns>DM_TG_MAX_AGE_NS || (s->selected_bank!=0 && s->selected_bank!=16))return g;
    uint32_t logical=0;
    switch(button){case DM_BTN_A:logical=0x2000;break;case DM_BTN_B:logical=0x1000;break;
        case DM_BTN_X:logical=0x8000;break;case DM_BTN_Y:logical=0x4000;break;case DM_BTN_R:logical=0x200;break;default:return g;}
    int slot=-1;
    for(int i=0;i<DM_TG_FACETS;i++)if(s->mapping[i].logical==logical){if(slot>=0)return g;slot=(int)(skill_set?s->mapping[i].alt:s->mapping[i].normal);}
    if(slot<0 || slot>=(skill_set?12:6))return g;
    g=dm_ta_skill(s->raw_skill_id[slot]);g.slot=slot;g.effective_slot=s->selected_bank+slot;return g;
}

