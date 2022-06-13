#include "bsreq.h"
#include "crt.h"
#include "stringbuilder.h"
#include "resid.h"

BSMETA(residi) = {
	BSREQ(id),
	{}};
BSDATA(residi) = {
	{"NONE"},
	{"BACKGRND"}, {"SKILLDEX"}, {"INVEN"}, {"ITEMS"},
	{"MISC"}, {"SCENERY"}, {"WALLS"}, {"TILES"}, {"INTRFACE"},
	{"FONT1"}, {"FONT2"}, {"FONT3"}, {"FONT4"},
	{"HANPWR"}, {"HAPOWR"}, {"HAROBE"},
	{"HFCMBT"}, {"HFJMPS"}, {"HFLTHR"}, {"HFMAXX"}, {"HFMETL"}, {"HFPRIM"},
	{"HMBJMP"}, {"HMBMET"},
	{"HMCMBT"}, {"HMJMPS"}, {"HMLTHR"}, {"HMMAXX"}, {"HMMETL"}, {"HMWARR"},
	{"MABOS2"}, {"MABOSS"}, {"MAMTNT"}, {"MAMTN2"},
	{"MAROBE"}, {"NABLUE"}, {"NABRWN"},
	{"NACHLD"},
	{"NAGHUL"}, {"NAGLOW"}, {"NAPOWR"}, {"NAROBE"}, {"NAVGUL"}, {"NAWHIT"},
	{"NFASIA"},
	{"NFBRLP"}, {"NFLYNN"},
	{"NFMAXX"}, {"NFMETL"},
	{"NFNICE"}, {"NFPEAS"}, {"NFPRIM"}, {"NFTRMP"}, {"NFVALT"}, {"NFVRED"},
	{"NMASIA"},
	{"NMBONC"}, {"NMBOXX"}, {"NMBPEA"}, {"NMBRLP"}, {"NMBRSR"}, {"NMBSNP"},
	{"NMCOPP"}, {"NMDOCC"}, {"NMFATT"}, {"NMGANG"}, {"NMGRCH"}, {"NMLABB"},
	{"NMLOSR"}, {"NMLTHR"}, {"NMMAXX"}, {"NMMEXI"}, {"NMMYRN"}, {"NMNICE"}, {"NMOLDD"},
	{"NMPEAS"}, {"NMRGNG"}, {"NMVALT"}, {"NMWARR"},
	{"MAANTT"}, {"MABRAN"}, {"MABROM"}, {"MACLAW"}, {"MACLW2"}, {"MADDOG"}, {"MADEGG"},
	{"MADETH"}, {"MAFIRE"}, {"MAGCKO"}, {"MAGKO2"}, {"MALIEN"}, {"MAMANT"}, {"MAMRAT"},
	{"MAMURT"}, {"MAPLNT"}, {"MAQUEN"}, {"MASCP2"}, {"MASCRP"}, {"MASPHN"}, {"MASRAT"},
	{"MATHNG"}, {"MACYBR"}, {"MAFEYE"}, {"MAGUN2"}, {"MAGUNN"}, {"MAHAND"}, {"MAROBO"},
	{"MAROBT"},
};
assert_enum(residi, res::MAROBT)

sprite* gres(res id) {
	auto& e = bsdata<residi>::elements[(int)id];
	if(e.notfound)
		return 0;
	if(!e.data) {
		if(id==res::None)
			return 0;
		char temp[260]; stringbuilder sb(temp);
		sb.add("art/%1.pma", e.id);
		e.data = (sprite*)loadb(temp);
		e.notfound = !e.data;
	}
	return e.data;
}

const char* getresname(res id) {
	return bsdata<residi>::elements[(int)id].id;
}