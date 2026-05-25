//
// Created by Emmanuel Addo-Odame on 24/05/2026.
//

#ifndef NOSAPI_EMAILSERVICE_H
#define NOSAPI_EMAILSERVICE_H

#include "dto/BaseApiResponse.h"
#include "dto/SendEmailDto.h"
#include <drogon/drogon.h>

namespace nos::services {

    class EmailService {

        drogon::Task<dto::BaseApiResponse> sendEmail(const dto::SendEmailDto &dto);

    };

}
#endif //NOSAPI_EMAILSERVICE_H