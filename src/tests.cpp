#include "capbot/tests.h"

void db_test(Db db) {
    // DB Tests
    db.put({
        {
            {"key", "bastard1"},
            {"field1", "value1"}
        },
        {
            {"key", "bastard2"},
            {"field1", "value2"}
        }
    });
    db.post({
        {"key", "bastard3"},
        {"field1", "value3"}
    });
    db.patch("bastard2", {
        {"set", {
            {"field1", "value4"}
        }}
    });
    db.get("bastard1");
    db.get("bastard2");
    db.query(R"(
        [
            {
                "field1": "value3"
            }
        ]
    )"_json);
    db.del("bastard1");
    db.del("bastard2");
    db.del("bastard3");
}