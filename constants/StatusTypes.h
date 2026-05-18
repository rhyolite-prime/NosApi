//
// Created by Emmanuel Addo-Odame on 13/05/2026.
//

#ifndef NOSAPI_STATUS_H
#define NOSAPI_STATUS_H
namespace nos::constants {

    enum StatusTypes {

        Unpublished = 0,
        Published = 1,
        Archived = 2,
        Deleted = 3,
        Enabled = 4,
        Disabled = 5

    };

}
#endif //NOSAPI_STATUS_H